#pragma once

#include "EntityManager.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include "SimpleShader.h"

struct Material : ECS::Component
{
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedo;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normals;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalness;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughness;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ao;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

    std::string albedoName;
    std::string normalsName;
    std::string metalnessName;
    std::string roughnessName;
    std::string aoName;

    std::string pixelShaderName;
    std::string vertexShaderName;

    SimplePixelShader* pixelShader;
    SimpleVertexShader* vertexShader;

    DirectX::XMFLOAT3 tint = { 1, 1, 1 };

    virtual ~Material() {}

    static int id;
    virtual int ID()
    {
        return id;
    }

    virtual int Size()
    {
        return sizeof(Material);
    }
};