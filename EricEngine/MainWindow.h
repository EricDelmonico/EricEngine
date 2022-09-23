#pragma once

#include <Windows.h>

class MainWindow
{
public:
    MainWindow(HINSTANCE hInstance, int width, int height, int showCmd);
    ~MainWindow();
    HRESULT InitializeWindow();

    static MainWindow* MainWindowInstance;
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND GetWindow() { return m_hWnd; }

private:
    HINSTANCE m_hInstance;
    int m_width, m_height, m_showCmd;
    HWND m_hWnd;

    const wchar_t CLASS_NAME[22] = L"Direct3D Window Class";

};

