#include "CameraControl.h"
#include "Input.h"
#include <DirectXMath.h>
#include "EntityManager.h"
#include "TransformSystem.h"

using namespace DirectX;
using namespace ECS;

CameraControl::CameraControl(HWND hWnd, int windowWidth, int windowHeight)
{
    // Hide the cursor in release mode, also lock it to the window
#ifndef _DEBUG
    ShowCursor(FALSE);
    RECT rect = {};
    GetClientRect(hWnd, &rect);
    MapWindowPoints(hWnd, nullptr, (POINT*)(&rect), 2);
    ClipCursor(&rect);
    screenCenter = {};
    screenCenter.x = rect.left + (windowWidth / 2);
    screenCenter.y = rect.top + (windowHeight / 2);
#endif
}

void CameraControl::UpdateViewMatrix(Camera* c, Transform* t)
{
    XMFLOAT3 pos = t->position;
    XMFLOAT3 forward = t->forward;

    XMMATRIX v = XMMatrixLookToRH(
        XMLoadFloat3(&pos),         // OldCamera's position
        XMLoadFloat3(&forward),     // OldCamera's forward vector
        XMVectorSet(0, 1, 0, 0));   // World up (Y)

    XMStoreFloat4x4(&c->viewMatrix, v);
}

void CameraControl::UpdateProjectionMatrix(Camera* c)
{
    XMMATRIX p;
    if (c->perspective)
    {
        p = XMMatrixPerspectiveFovLH(
            c->fieldOfView,
            c->aspectRatio,
            0.01f,       // Near clip plane distance
            10000.0f);   // Far clip plane distance
    }
    else
    {
        p = XMMatrixOrthographicLH(
            c->orthoSize,
            c->orthoSize,
            0.01f,
            100.0f);
    }

    XMStoreFloat4x4(&c->projectionMatrix, p);
}

void CameraControl::Update(float dt)
{
    auto cameraEntities = EntityManager::GetInstance().GetEntitiesWithComponents<Camera, Transform>();
    if (cameraEntities.size() <= 0) return;

    // We just want one camera for camera control. Pick the first one.
    Camera* cam = EntityManager::GetInstance().GetComponent<Camera>(cameraEntities[0]);
    Transform* transform = EntityManager::GetInstance().GetComponent<Transform>(cameraEntities[0]);

    // Get a reference to the input manager
    Input& input = Input::GetInstance();

    // Calculate the current speed
    float speed = cam->movementSpeed * dt;

    // Movement
    auto forward = transform->forward;
    forward.y = 0;
    auto forwardNoY = XMVector3Normalize(XMLoadFloat3(&forward));
    XMStoreFloat3(&forward, forwardNoY);
    if (input.KeyDown(VK_LSHIFT)) { speed *= 2; }
#if _DEBUG
    if (input.KeyDown('W')) { TransformSystem::MoveRelative(transform, 0, 0, -speed); }
    if (input.KeyDown('S')) { TransformSystem::MoveRelative(transform, 0, 0, speed); }
#else
    // take away free fly in release mode
    if (input.KeyDown('W')) { TransformSystem::MoveAbsolute(transform, forward.x * -speed, 0, forward.z * -speed); }
    if (input.KeyDown('S')) { TransformSystem::MoveAbsolute(transform, forward.x * speed, 0, forward.z * speed); }
#endif
    if (input.KeyDown('A')) { TransformSystem::MoveRelative(transform, speed, 0, 0); }
    if (input.KeyDown('D')) { TransformSystem::MoveRelative(transform, -speed, 0, 0); }
    if (input.KeyDown('E')) { TransformSystem::MoveAbsolute(transform, 0, speed, 0); }
    if (input.KeyDown('Q')) { TransformSystem::MoveAbsolute(transform, 0, -speed, 0); }
    if (input.KeyDown('P')) { sensitivity += 0.1f * dt; }
    if (input.KeyDown('L')) { sensitivity -= 0.1f * dt; }

    // Don't let sensitivity go too low, especially below 0
    if (sensitivity < 0.05f) sensitivity = 0.05f;

    // Rotate if the mouse is down
#if _DEBUG
    if (input.MouseLeftDown())
#endif
    {
        // Calculate how much the cursor changed
        float xDiff = dt * cam->mouseLookSpeed * input.GetMouseXDelta() * sensitivity;
        float yDiff = dt * -cam->mouseLookSpeed * input.GetMouseYDelta() * sensitivity;

#ifndef _DEBUG
        // Stick cursor to the center of screen in release mode
        SetCursorPos(screenCenter.x, screenCenter.y);
        input.ResetPrevMousePos();
#endif

        // Don't allow pitch to go more than 90 degrees or less than -90 degrees
        XMFLOAT3 pitchYawRoll = transform->pitchYawRoll;
        if (pitchYawRoll.x + yDiff > DirectX::XM_PIDIV2 - 0.05f ||
            pitchYawRoll.x + yDiff < -DirectX::XM_PIDIV2 + 0.05f)
        {
            yDiff = 0;
        }

        // Rotate the transform! SWAP X AND Y!
        TransformSystem::Rotate(transform, yDiff, xDiff, 0);
    }

    auto pos = transform->position;
    TransformSystem::SetPosition(transform, pos.x, pos.y, pos.z);

    // At the end, update the view
    UpdateViewMatrix(cam, transform);
    UpdateProjectionMatrix(cam);
}

