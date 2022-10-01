#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "D3DResources.h"
#include <memory>

#pragma comment (lib, "d3d11.lib")

struct Mesh
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    int indices;
};