#include "AssetManager.h"
#include "Vertex.h"
#include "StringConversion.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#include "WICTextureLoader.h"

#include "boost/exception/all.hpp"

#include "DirectoryEnumeration.h"

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

AssetManager::~AssetManager()
{
    for (auto kvp : m_loadedMeshes)
    {
        delete kvp.second;
    }

    m_loadedMeshes.clear();
    m_loadedTextureSRVs.clear();
}

std::shared_ptr<SimplePixelShader> AssetManager::GetPixelShader(std::string name)
{
    if (m_pixelShaders.find(name) != m_pixelShaders.end()) return m_pixelShaders[name];

    auto device = m_d3dResources->GetDeviceComPtr();
    auto context = m_d3dResources->GetContextComPtr();
    m_pixelShaders.insert({ name, std::make_shared<SimplePixelShader>(device, context, StringConversion::StringToWString(DirectoryEnumeration::GetExePath() + name + ".cso").c_str())});
    return m_pixelShaders[name];
}

std::shared_ptr<SimpleVertexShader> AssetManager::GetVertexShader(std::string name)
{
    if (m_vertexShaders.find(name) != m_vertexShaders.end()) return m_vertexShaders[name];

    auto device = m_d3dResources->GetDeviceComPtr();
    auto context = m_d3dResources->GetContextComPtr();
    m_vertexShaders.insert({ name, std::make_shared<SimpleVertexShader>(device, context, StringConversion::StringToWString(DirectoryEnumeration::GetExePath() + name + ".cso").c_str()) });
    return m_vertexShaders[name];
}

Mesh* AssetManager::GetMesh(std::string name)
{
    if (m_loadedMeshes.find(name) != m_loadedMeshes.end()) return m_loadedMeshes[name];

    // Create an instance of the importer class
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(DirectoryEnumeration::GetExePath() + "../../Assets/Models/" + name,
        aiProcess_CalcTangentSpace      |
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded   |
        aiProcess_CalcTangentSpace      |
        aiProcess_PreTransformVertices);

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
        DirectX::XMFLOAT3 max = dxPositions[0];
        DirectX::XMFLOAT3 min = dxPositions[0];
        for (int i = 0; i < assimpMesh->mNumVertices; i++)
        {
            vertices.push_back({ dxPositions[i], dxNormals[i], dxTangents[i], DirectX::XMFLOAT2(dxUVs[i].x, dxUVs[i].y) });
            
            if (dxPositions[i].x > max.x) max.x = dxPositions[i].x;
            if (dxPositions[i].y > max.y) max.y = dxPositions[i].y;
            if (dxPositions[i].z > max.z) max.z = dxPositions[i].z;

            if (dxPositions[i].x < min.x) min.x = dxPositions[i].x;
            if (dxPositions[i].y < min.y) min.y = dxPositions[i].y;
            if (dxPositions[i].z < min.z) min.z = dxPositions[i].z;
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

        m_loadedMeshes.insert({ name, new Mesh() });
        auto mesh = m_loadedMeshes[name];
        mesh->vertexBuffer = vb;
        mesh->indexBuffer = ib;
        mesh->indices = numIndices;
        mesh->boundingMax = max;
        mesh->boundingMin = min;
        mesh->name = name;
        return m_loadedMeshes[name];
    }

    // If we got here, we did not have a mesh
    std::string errorMessage = "Failed to load model " + name;
    return nullptr;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> AssetManager::GetTexture(std::string name)
{
    if (m_loadedTextureSRVs.find(name) != m_loadedTextureSRVs.end()) return m_loadedTextureSRVs[name];
    
    // Load in the texture file
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    auto hr = CreateWICTextureFromFile(
        m_d3dResources->GetDevice(),
        m_d3dResources->GetContext(),
        StringConversion::StringToWString(DirectoryEnumeration::GetExePath() + "../../Assets/Textures/" + name).c_str(),
        nullptr,
        &m_loadedTextureSRVs[name]);


    if (hr == S_OK)
    {
        // Stick srv into our map
        m_loadedTextureSRVs.insert({ name, srv });
        // Return our texture
        return m_loadedTextureSRVs[name];
    }
    else 
    {
        // Texture failed to load :(
        return nullptr;
    }
}
