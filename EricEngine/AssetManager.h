#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <string>
#include <unordered_map>
#include "D3DResources.h"
#include <memory>
#include "SimpleShader.h"
#include "Mesh.h"

class AssetManager
{
public:
    AssetManager(std::shared_ptr<D3DResources> d3dResources);
    ~AssetManager();

    SimplePixelShader* GetPixelShader(std::string name);
    SimpleVertexShader* GetVertexShader(std::string name);
    /// <summary>
    /// Retrieves a mesh from the AssetManager or loads in the file
    /// 
    /// NOTE: For now, only supports a simple scene with only one mesh.
    /// Does not load in any material data at the moment
    /// </summary>
    /// <param name="name">The name of the model to load in, including any file extension</param>
    /// <returns>A pointer to the loaded in mesh</returns>
    Mesh* GetMesh(std::string name);

    /// <summary>
    /// Gets a ShaderResourceView containing the texture named std::string name
    /// </summary>
    /// <param name="name">The name of the texture file, including file extension</param>
    /// <returns>Shader resource view </returns>
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(std::string name);

    Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState() { return m_basicSamplerState; }

    /// <summary>
    /// Should only be called using a loaded Mesh's name
    /// </summary>
    /// <param name="name">The name of the Mesh this index buffer belongs to</param>
    /// <returns></returns>
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer(std::string name);
    /// <summary>
    /// Should only be called using a loaded Mesh's name
    /// </summary>
    /// <param name="name">The name of the Mesh this vertex buffer belongs to</param>
    /// <returns></returns>
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer(std::string name);

private:
    std::shared_ptr<D3DResources> m_d3dResources;

    std::unordered_map<std::string, std::unique_ptr<SimplePixelShader>> m_pixelShaders;
    std::unordered_map<std::string, std::unique_ptr<SimpleVertexShader>> m_vertexShaders;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_loadedTextureSRVs;

    std::unordered_map<std::string, Mesh> m_loadedMeshes;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>> m_loadedVertexBuffers;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>> m_loadedIndexBuffers;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_basicSamplerState;
};
