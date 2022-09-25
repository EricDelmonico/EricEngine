#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <string>
#include <unordered_map>
#include "D3DResources.h"
#include <memory>
#include "SimpleShader.h"

class AssetManager
{
public:
    AssetManager(std::shared_ptr<D3DResources> d3dResources);

    std::shared_ptr<SimplePixelShader> GetPixelShader(std::wstring name);
    std::shared_ptr<SimpleVertexShader> GetVertexShader(std::wstring name);

private:
    std::shared_ptr<D3DResources> m_d3dResources;

    std::unordered_map<std::wstring, std::shared_ptr<SimplePixelShader>> m_pixelShaders;
    std::unordered_map<std::wstring, std::shared_ptr<SimpleVertexShader>> m_vertexShaders;

    std::wstring GetExePathLong();
    std::string GetExePath();

    void ImportAsset();
};
