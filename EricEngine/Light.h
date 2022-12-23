#pragma once

#include "EntityManager.h"
#include <DirectXMath.h>

#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1

struct Light
{
    DirectX::XMFLOAT3 dir;
    float intensity;
    DirectX::XMFLOAT3 color;
    int lightType;
    DirectX::XMFLOAT3 pos;
    float range;
};

struct LightComponent : ECS::Component
{
    Light data = {};

    virtual ~LightComponent() {}

    static int id;
    virtual int ID()
    {
        return id;
    }
};

