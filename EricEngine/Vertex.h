#pragma once

#include <DirectXMath.h>

struct Vertex 
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT3 Normal;
    DirectX::XMFLOAT3 Tangent;
    DirectX::XMFLOAT2 UV;
};