#pragma once
#include <Windows.h>
#include "Camera.h"
#include "Transform.h"

class CameraControl
{
public:
    void Update(float dt);
    CameraControl(HWND hWnd, int windowWidth, int windowHeight);

private:
    float sensitivity = 0.1f;
    POINT screenCenter;

    void UpdateViewMatrix(Camera* c, Transform* t);
    void UpdateProjectionMatrix(Camera* c);
};
