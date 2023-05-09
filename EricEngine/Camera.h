#pragma once
#include <DirectXMath.h>
#include "Transform.h"
#include "EntityManager.h"

struct Camera : ECS::Component
{
public:
    // Camera matrices
    DirectX::XMFLOAT4X4 viewMatrix;
    DirectX::XMFLOAT4X4 projectionMatrix;

    float movementSpeed;
    float mouseLookSpeed;
    float fieldOfView;
    float aspectRatio;

    bool perspective;

    float orthoSize;

    virtual ~Camera();

    static int id;
    virtual int ID()
    {
        return id;
    }
};

