#pragma once

#include "EntityManager.h"
#include <DirectXMath.h>

struct Light : ECS::Component
{
    DirectX::XMFLOAT3 dir;
    DirectX::XMFLOAT3 color;
    float intensity;

    virtual ~Light() {}

    static int id;
    virtual int ID()
    {
        return id;
    }

    virtual int Size()
    {
        return sizeof(Light);
    }
};

