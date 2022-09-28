#include "AssetManager.h"
#include "VertexPositionColor.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <DirectXMath.h>

AssetManager::AssetManager(std::shared_ptr<D3DResources> d3dResources) : m_d3dResources(d3dResources)
{
}

std::shared_ptr<SimplePixelShader> AssetManager::GetPixelShader(std::wstring name)
{
    if (m_pixelShaders.find(name) != m_pixelShaders.end()) return m_pixelShaders[name];

    auto device = m_d3dResources->GetDeviceComPtr();
    auto context = m_d3dResources->GetContextComPtr();
    m_pixelShaders.insert({ name, std::make_shared<SimplePixelShader>(device, context, (AssetManager::GetExePathLong() + name + L".cso").c_str())});
    return m_pixelShaders[name];
}

std::shared_ptr<SimpleVertexShader> AssetManager::GetVertexShader(std::wstring name)
{
    if (m_vertexShaders.find(name) != m_vertexShaders.end()) return m_vertexShaders[name];

    auto device = m_d3dResources->GetDeviceComPtr();
    auto context = m_d3dResources->GetContextComPtr();
    m_vertexShaders.insert({ name, std::make_shared<SimpleVertexShader>(device, context, (AssetManager::GetExePathLong() + name + L".cso").c_str()) });
    return m_vertexShaders[name];
}

std::wstring AssetManager::GetExePathLong()
{
    // Assume the path is just the "current directory" for now
    std::wstring path = L".\\";

    // Get the real, full path to this executable
    wchar_t currentDir[1024] = {};
    GetModuleFileName(0, currentDir, 1024);

    // Find the location of the last slash charaacter
    wchar_t* lastSlash = wcsrchr(currentDir, '\\');
    if (lastSlash)
    {
        // End the string at the last slash character, essentially
        // chopping off the exe's file name.  Remember, c-strings
        // are null-terminated, so putting a "zero" character in 
        // there simply denotes the end of the string.
        *lastSlash = 0;

        // Set the remainder as the path
        path = currentDir;
        path += L"\\";
    }

    // Toss back whatever we've found
    return path;
}

std::string AssetManager::GetExePath()
{
    // Assume the path is just the "current directory" for now
    std::string path = ".\\";

    // Get the real, full path to this executable
    char currentDir[1024] = {};
    GetModuleFileNameA(0, currentDir, 1024);

    // Find the location of the last slash charaacter
    char* lastSlash = strrchr(currentDir, '\\');
    if (lastSlash)
    {
        // End the string at the last slash character, essentially
        // chopping off the exe's file name.  Remember, c-strings
        // are null-terminated, so putting a "zero" character in 
        // there simply denotes the end of the string.
        *lastSlash = 0;

        // Set the remainder as the path
        path = currentDir;
        path += "\\";
    }

    // Toss back whatever we've found
    return path;
}

std::shared_ptr<Mesh> AssetManager::LoadMesh(std::string name)
{
    // Create an instance of the importer class
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(AssetManager::GetExePath() + "../../Assets/Models/" + name,
        aiProcess_CalcTangentSpace      |
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipWindingOrder      |
        aiProcess_ConvertToLeftHanded);

    // We don't have a scene, we don't have a mesh
    if (!scene) return nullptr;

    if (scene->HasMeshes())
    {
        // Vertex and index buffers
        Microsoft::WRL::ComPtr<ID3D11Buffer> vb;
        Microsoft::WRL::ComPtr<ID3D11Buffer> ib;

        // Grab the mesh
        auto assimpMesh = scene->mMeshes[0];
        
        // Turn mesh verts into our vertex struct
        DirectX::XMFLOAT3* dxPositions = (DirectX::XMFLOAT3*)((void*)assimpMesh->mVertices);
        std::vector<VertexPositionColor> vertices;
        for (int i = 0; i < assimpMesh->mNumVertices; i++)
        {
            vertices.push_back({ dxPositions[i], DirectX::XMFLOAT3(0.5f, 0.0f, 0.5f) });
        }

        // Set up vertex buffer
        CD3D11_BUFFER_DESC vDesc(assimpMesh->mNumVertices * sizeof(VertexPositionColor), D3D11_BIND_VERTEX_BUFFER);
        D3D11_SUBRESOURCE_DATA vData = {};
        vData.pSysMem = &vertices[0];
        vData.SysMemPitch = 0;
        vData.SysMemSlicePitch = 0;

        m_d3dResources->GetDevice()->CreateBuffer(&vDesc, &vData, &vb);

        // Set up indices
        int numIndices = assimpMesh->mNumFaces * 3;
        std::vector<unsigned int> indices;
        int currentIndex = 0;
        for (int i = 0; i < assimpMesh->mNumFaces; i++) 
        {
            indices.push_back(assimpMesh->mFaces[i].mIndices[0]);
            indices.push_back(assimpMesh->mFaces[i].mIndices[1]);
            indices.push_back(assimpMesh->mFaces[i].mIndices[2]);
        }

        // Set up index buffer
        CD3D11_BUFFER_DESC iDesc(numIndices * sizeof(int), D3D11_BIND_INDEX_BUFFER);
        D3D11_SUBRESOURCE_DATA iData = {};
        iData.pSysMem = &indices[0];
        iData.SysMemPitch = 0;
        iData.SysMemSlicePitch = 0;

        m_d3dResources->GetDevice()->CreateBuffer(&iDesc, &iData, &ib);

        return std::make_shared<Mesh>(m_d3dResources, vb, ib, numIndices);
    }

    // If we got here, we did not have a mesh
    return nullptr;
}