#pragma once

#include "EntityManager.h"
#include <d3d11.h>
#include <wrl/client.h>

struct Material : ECS::Component
{
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedo;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normals;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalness;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughness;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

    static int id;
    virtual int ID()
    {
        return id;
    }
};