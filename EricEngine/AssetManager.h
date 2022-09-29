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

    std::shared_ptr<SimplePixelShader> GetPixelShader(std::wstring name);
    std::shared_ptr<SimpleVertexShader> GetVertexShader(std::wstring name);
    /// <summary>
    /// Retrieves a mesh from the AssetManager or loads in the file
    /// 
    /// NOTE: For now, only supports a simple scene with only one mesh.
    /// Does not load in any material data at the moment
    /// </summary>
    /// <param name="name">The name of the model to load in, including any file extension</param>
    /// <returns>A pointer to the loaded in mesh</returns>
    std::shared_ptr<Mesh> GetMesh(std::string name);

    /// <summary>
    /// Gets a ShaderResourceView containing the texture named std::string name
    /// </summary>
    /// <param name="name">The name of the texture file, including file extension</param>
    /// <returns>Shader resource view </returns>
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(std::wstring name);

    Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState() { return m_basicSamplerState; }

private:
    std::shared_ptr<D3DResources> m_d3dResources;

    std::unordered_map<std::wstring, std::shared_ptr<SimplePixelShader>> m_pixelShaders;
    std::unordered_map<std::wstring, std::shared_ptr<SimpleVertexShader>> m_vertexShaders;

    std::wstring GetExePathLong();
    std::string GetExePath();

    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_loadedTextureSRVs;

    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_loadedMeshes;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_basicSamplerState;
};
