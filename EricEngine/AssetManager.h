#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <string>
#include <unordered_map>
#include "D3DResources.h"
#include <memory>

class AssetManager
{
public:
    AssetManager(std::shared_ptr<D3DResources> d3dResources);

    Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader(std::string name);
    Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader(std::string name);
    Microsoft::WRL::ComPtr<ID3D11InputLayout> GetInputLayout(std::string name) { return m_inputLayouts[name]; }

private:
    std::shared_ptr<D3DResources> m_d3dResources;

    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> m_pixelShaders;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11VertexShader>> m_vertexShaders;
    std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11InputLayout>> m_inputLayouts;

    std::string GetExePath();
};
