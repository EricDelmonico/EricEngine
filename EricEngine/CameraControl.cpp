#include "CameraControl.h"
#include "Camera.h"
#include "Input.h"
#include <DirectXMath.h>
#include "EntityManager.h"

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

void CameraControl::Update(float dt)
{
    auto cameraEntities = EntityManager::GetInstance().GetEntitiesWithComponents<Camera>();
    if (cameraEntities.size() <= 0) return;

    // We just want one camera for camera control. Pick the first one.
    Camera* cam = EntityManager::GetInstance().GetComponent<Camera>(cameraEntities[0]);

    // Get a reference to the input manager
    Input& input = Input::GetInstance();

    // Calculate the current speed
    float speed = cam->movementSpeed * dt;

    // Movement
    auto forward = cam->transform.GetForward();
    forward.y = 0;
    auto forwardNoY = XMVector3Normalize(XMLoadFloat3(&forward));
    XMStoreFloat3(&forward, forwardNoY);
    if (input.KeyDown(VK_LSHIFT)) { speed *= 2; }
#if _DEBUG
    if (input.KeyDown('W')) { cam->transform.MoveRelative(0, 0, -speed); }
    if (input.KeyDown('S')) { cam->transform.MoveRelative(0, 0, speed); }
#else
    // take away free fly in release mode
    if (input.KeyDown('W')) { cam->transform.MoveAbsolute(forward.x * -speed, 0, forward.z * -speed); }
    if (input.KeyDown('S')) { cam->transform.MoveAbsolute(forward.x * speed, 0, forward.z * speed); }
#endif
    if (input.KeyDown('A')) { cam->transform.MoveRelative(speed, 0, 0); }
    if (input.KeyDown('D')) { cam->transform.MoveRelative(-speed, 0, 0); }
    if (input.KeyDown('E')) { cam->transform.MoveAbsolute(0, speed, 0); }
    if (input.KeyDown('Q')) { cam->transform.MoveAbsolute(0, -speed, 0); }
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
        XMFLOAT3 pitchYawRoll = cam->transform.GetPitchYawRoll();
        if (pitchYawRoll.x + yDiff > DirectX::XM_PIDIV2 - 0.05f ||
            pitchYawRoll.x + yDiff < -DirectX::XM_PIDIV2 + 0.05f)
        {
            yDiff = 0;
        }

        // Rotate the transform! SWAP X AND Y!
        cam->transform.Rotate(yDiff, xDiff, 0);
    }

    auto pos = cam->transform.GetPosition();
    cam->transform.SetPosition(pos.x, pos.y, pos.z);

    // At the end, update the view
    cam->UpdateViewMatrix();
}

