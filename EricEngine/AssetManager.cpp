#include "AssetManager.h"
#include "Vertex.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#include "WICTextureLoader.h"

AssetManager::AssetManager(std::shared_ptr<D3DResources> d3dResources) : m_d3dResources(d3dResources)
{
    // Sampler description/sampler state
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    d3dResources->GetDevice()->CreateSamplerState(&samplerDesc, m_basicSamplerState.GetAddressOf());
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

std::shared_ptr<Mesh> AssetManager::GetMesh(std::string name)
{
    if (m_loadedMeshes.find(name) != m_loadedMeshes.end()) return m_loadedMeshes[name];

    // Create an instance of the importer class
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(AssetManager::GetExePath() + "../../Assets/Models/" + name,
        aiProcess_CalcTangentSpace      |
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded   |
        aiProcess_CalcTangentSpace);

    // We don't have a scene, we don't have a mesh
    if (!scene) return nullptr;

    if (scene->HasMeshes())
    {
        // Vertex and index buffers
        Microsoft::WRL::ComPtr<ID3D11Buffer> vb;
        Microsoft::WRL::ComPtr<ID3D11Buffer> ib;

        // Grab the mesh
        auto assimpMesh = scene->mMeshes[0];
        
        // Reinterpret vectors as XMFLOAT equivalents
        DirectX::XMFLOAT3* dxPositions = (DirectX::XMFLOAT3*)((void*)assimpMesh->mVertices);
        DirectX::XMFLOAT3* dxNormals = (DirectX::XMFLOAT3*)((void*)assimpMesh->mNormals);
        DirectX::XMFLOAT3* dxTangents = (DirectX::XMFLOAT3*)((void*)assimpMesh->mTangents);
        DirectX::XMFLOAT3* dxUVs = (DirectX::XMFLOAT3*)((void*)assimpMesh->mTextureCoords[0]);

        // Turn mesh verts into our vertex struct
        //
        // Vertex format is:
        // float3 Position
        // float3 Normal
        // float3 Tangent
        // float2 UV
        std::vector<Vertex> vertices;
        for (int i = 0; i < assimpMesh->mNumVertices; i++)
        {
            vertices.push_back({ dxPositions[i], dxNormals[i], dxTangents[i], DirectX::XMFLOAT2(dxUVs[i].x, dxUVs[i].y) });
        }

        // Set up vertex buffer
        CD3D11_BUFFER_DESC vDesc(assimpMesh->mNumVertices * sizeof(Vertex), D3D11_BIND_VERTEX_BUFFER);
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

        m_loadedMeshes.insert({ name, std::make_shared<Mesh>(m_d3dResources, vb, ib, numIndices) });
        return m_loadedMeshes[name];
    }

    // If we got here, we did not have a mesh
    return nullptr;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> AssetManager::GetTexture(std::wstring name)
{
    if (m_loadedTextureSRVs.find(name) != m_loadedTextureSRVs.end()) return m_loadedTextureSRVs[name];
    
    // Stick srv into our map
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    m_loadedTextureSRVs.insert({ name, srv });

    // Load in the texture file
    auto hr = CreateWICTextureFromFile(
        m_d3dResources->GetDevice(),
        m_d3dResources->GetContext(),
        (AssetManager::GetExePathLong() + L"../../Assets/Textures/" + name).c_str(),
        nullptr,
        &m_loadedTextureSRVs[name]);

    // Return our texture
    return m_loadedTextureSRVs[name];
}
