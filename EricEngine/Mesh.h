#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "D3DResources.h"
#include <memory>

#pragma comment (lib, "d3d11.lib")

class Mesh
{
public:
    Mesh(std::shared_ptr<D3DResources> d3dResources, Microsoft::WRL::ComPtr<ID3D11Buffer> vb, Microsoft::WRL::ComPtr<ID3D11Buffer> ib, int indices);
    static Mesh CreateCube(std::shared_ptr<D3DResources> d3dResources);

    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return vertexBuffer; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() { return indexBuffer; }
    int GetNumIndices() { return indices; }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    std::shared_ptr<D3DResources> m_d3dResources;

    int indices;
};

