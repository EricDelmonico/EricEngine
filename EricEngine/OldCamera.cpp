#include "OldCamera.h"
#include <iostream>

using namespace DirectX;

int OldCamera::id;

OldCamera::OldCamera()
{
}

OldCamera::OldCamera(
    float x,
    float y,
    float z,
    float moveSpeed,
    float lookSpeed,
    float fov,
    float aspectRatio,
    bool perspective,
    float orthoSize,
    float pitch,
    float yaw,
    float roll) :
    movementSpeed(moveSpeed),
    mouseLookSpeed(lookSpeed),
    fieldOfView(fov),
    aspectRatio(aspectRatio),
    perspective(perspective),
    orthoSize(orthoSize)
{
    // Set up the transform
    transform.SetPosition(x, y, z);
    transform.SetPitchYawRoll(pitch, yaw, roll);

    // Set up our matrices
    UpdateViewMatrix();
    UpdateProjectionMatrix(aspectRatio);
}

OldCamera::OldCamera(
    Transform transform,
    float moveSpeed,
    float lookSpeed,
    float fov,
    float aspectRatio,
    bool perspective,
    float orthoSize) :
    transform(transform),
    movementSpeed(moveSpeed),
    mouseLookSpeed(lookSpeed),
    fieldOfView(fov),
    aspectRatio(aspectRatio),
    perspective(perspective),
    orthoSize(orthoSize)
{
    UpdateViewMatrix();
    UpdateProjectionMatrix(aspectRatio);
}

OldCamera::~OldCamera()
{
}

void OldCamera::UpdateViewMatrix()
{
    XMFLOAT3 pos = transform.GetPosition();
    XMFLOAT3 forward = transform.GetForward();

    XMMATRIX v = XMMatrixLookToRH(
        XMLoadFloat3(&pos),         // OldCamera's position
        XMLoadFloat3(&forward),     // OldCamera's forward vector
        XMVectorSet(0, 1, 0, 0));   // World up (Y)

    XMStoreFloat4x4(&viewMatrix, v);
}

void OldCamera::UpdateProjectionMatrix(float aspectRatio)
{
    XMMATRIX p;
    if (perspective)
    {
        p = XMMatrixPerspectiveFovLH(
            fieldOfView,
            aspectRatio,
            0.01f,       // Near clip plane distance
            10000.0f);   // Far clip plane distance
    }
    else
    {
        p = XMMatrixOrthographicLH(
            orthoSize,
            orthoSize,
            0.01f,
            100.0f);
    }

    XMStoreFloat4x4(&projectionMatrix, p);
}

DirectX::XMFLOAT4X4 OldCamera::GetView()
{
    return viewMatrix;
}

DirectX::XMFLOAT4X4 OldCamera::GetProjection()
{
    return projectionMatrix;
}

Transform* OldCamera::GetTransform()
{
    return &transform;
}

float OldCamera::GetFoV()
{
    return fieldOfView;
}

void OldCamera::SetFoV(float fov)
{
    fieldOfView = fov;
    UpdateProjectionMatrix(aspectRatio);
}

float OldCamera::GetOrthoSize()
{
    return orthoSize;
}

void OldCamera::SetOrthoSize(float size)
{
    orthoSize = size;
}