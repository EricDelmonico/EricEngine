#pragma once
#include <Windows.h>

class CameraControl
{
public:
    void Update(float dt);
    CameraControl(HWND hWnd, int windowWidth, int windowHeight);

private:
    float sensitivity = 1.0f;
    POINT screenCenter;
};

