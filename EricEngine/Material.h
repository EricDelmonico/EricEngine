#pragma once

#include "EntityManager.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include "SimpleShader.h"

struct Material : ECS::Component
{
    std::string albedoName;
    std::string normalsName;
    std::string metalnessName;
    std::string roughnessName;
    std::string aoName;

    std::string pixelShaderName;
    std::string vertexShaderName;

    DirectX::XMFLOAT3 tint = { 1, 1, 1 };

    virtual ~Material() {}

    static int id;
    virtual int ID()
    {
        return id;
    }
};