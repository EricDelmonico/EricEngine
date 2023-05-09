#pragma once
#include <DirectXMath.h>
#include "EntityManager.h"

struct Transform : ECS::Component
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 pitchYawRoll;
    DirectX::XMFLOAT3 scale;
    DirectX::XMFLOAT4X4 worldMatrix;
    DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
    bool matricesDirty;
    DirectX::XMFLOAT3 up;
    DirectX::XMFLOAT3 right;
    DirectX::XMFLOAT3 forward;

// Pre-ECS Transform. Want to keep temporarily for later system implementations
public:
    Transform();
    virtual ~Transform();

    static int id;
    virtual int ID()
    {
        return id;
    }
};
