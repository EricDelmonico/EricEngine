#include "Camera.h"
#include <iostream>

using namespace DirectX;

int Camera::id;

Camera::Camera()
{
}

Camera::Camera(
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

Camera::Camera(
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

Camera::~Camera()
{
}

void Camera::UpdateViewMatrix()
{
    XMFLOAT3 pos = transform.GetPosition();
    XMFLOAT3 forward = transform.GetForward();

    XMMATRIX v = XMMatrixLookToRH(
        XMLoadFloat3(&pos),         // Camera's position
        XMLoadFloat3(&forward),     // Camera's forward vector
        XMVectorSet(0, 1, 0, 0));   // World up (Y)

    XMStoreFloat4x4(&viewMatrix, v);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
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

DirectX::XMFLOAT4X4 Camera::GetView()
{
    return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjection()
{
    return projectionMatrix;
}

Transform* Camera::GetTransform()
{
    return &transform;
}

float Camera::GetFoV()
{
    return fieldOfView;
}

void Camera::SetFoV(float fov)
{
    fieldOfView = fov;
    UpdateProjectionMatrix(aspectRatio);
}

float Camera::GetOrthoSize()
{
    return orthoSize;
}

void Camera::SetOrthoSize(float size)
{
    orthoSize = size;
}
