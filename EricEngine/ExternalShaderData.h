#pragma once
#include <DirectXMath.h>

struct ExternalData
{
    DirectX::XMFLOAT4X4 Model;
    DirectX::XMFLOAT4X4 View;
    DirectX::XMFLOAT4X4 Projection;
};