#pragma once
#include <Windows.h>

class CameraControl
{
public:
    void Update(float dt);
    CameraControl(HWND hWnd, int windowWidth, int windowHeight);

private:
    float sensitivity = 0.1f;
    POINT screenCenter;
};
