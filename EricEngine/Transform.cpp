#include "Transform.h"

using namespace DirectX;

int Transform::id;

Transform::Transform()
{
    position = XMFLOAT3(0, 0, 0);
    pitchYawRoll = XMFLOAT3(0, 0, 0);
    scale = XMFLOAT3(1, 1, 1);

    XMMATRIX ident = XMMatrixIdentity();
    XMStoreFloat4x4(&worldMatrix, ident);
    XMStoreFloat4x4(&worldInverseTransposeMatrix, ident);

    matricesDirty = false;
}

Transform::~Transform()
{
}