#include "CameraControl.h"
#include "Camera.h"
#include "Input.h"
#include <DirectXMath.h>
#include "EntityManager.h"

using namespace DirectX;
using namespace ECS;

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
    if (input.KeyDown(VK_LSHIFT)) { speed *= 2; }
    if (input.KeyDown('W')) { cam->transform.MoveRelative(0, 0, -speed); }
    if (input.KeyDown('S')) { cam->transform.MoveRelative(0, 0, speed); }
    if (input.KeyDown('A')) { cam->transform.MoveRelative(speed, 0, 0); }
    if (input.KeyDown('D')) { cam->transform.MoveRelative(-speed, 0, 0); }
    if (input.KeyDown('E')) { cam->transform.MoveAbsolute(0, speed, 0); }
    if (input.KeyDown('Q')) { cam->transform.MoveAbsolute(0, -speed, 0); }

    // Rotate if the mouse is down
    if (input.MouseLeftDown())
    {
        // Calculate how much the cursor changed
        float xDiff = dt * cam->mouseLookSpeed * input.GetMouseXDelta();
        float yDiff = dt * -cam->mouseLookSpeed * input.GetMouseYDelta();

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
    cam->transform.SetPosition(pos.x, 30, pos.z);

    // At the end, update the view
    cam->UpdateViewMatrix();
}

