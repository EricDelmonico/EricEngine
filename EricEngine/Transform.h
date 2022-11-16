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

// Pre-ECS Transform. Want to keep temporarily for later system implementations
public:
    Transform();
    virtual ~Transform();

    // Getters
    DirectX::XMFLOAT3 GetUp();
    DirectX::XMFLOAT3 GetRight();
    DirectX::XMFLOAT3 GetForward();

    DirectX::XMFLOAT3 GetPosition();
    DirectX::XMFLOAT3 GetPitchYawRoll();
    DirectX::XMFLOAT3 GetScale();

    DirectX::XMFLOAT4X4 GetWorldMatrix();
    DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();

    // Setters
    void SetPosition(float x, float y, float z);
    void SetPitchYawRoll(float pitch, float yaw, float roll);
    void SetScale(float x, float y, float z);

    // Transformers
    void MoveAbsolute(float x, float y, float z);
    void MoveRelative(float x, float y, float z);
    void Rotate(float pitch, float yaw, float roll);
    void Scale(float x, float y, float z);

    static int id;
    virtual int ID()
    {
        return id;
    }

    virtual int Size()
    {
        return sizeof(Transform);
    }

private:

    // Helper for updating matrices
    void UpdateMatrices();
};
