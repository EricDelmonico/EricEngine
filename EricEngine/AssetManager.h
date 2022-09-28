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
    /// Load in a mesh. NOTE: For now, only supports a simple scene with only one mesh, and no bells and whistles such as UV's, normals, or tangents.
    /// </summary>
    /// <returns></returns>
    std::shared_ptr<Mesh> LoadMesh(std::string name);

private:
    std::shared_ptr<D3DResources> m_d3dResources;

    std::unordered_map<std::wstring, std::shared_ptr<SimplePixelShader>> m_pixelShaders;
    std::unordered_map<std::wstring, std::shared_ptr<SimpleVertexShader>> m_vertexShaders;

    std::wstring GetExePathLong();
    std::string GetExePath();
};
