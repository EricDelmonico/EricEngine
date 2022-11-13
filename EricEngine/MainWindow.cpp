#include "MainWindow.h"
#include "Input.h"

#ifdef _DEBUG
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#endif

MainWindow* MainWindow::MainWindowInstance = nullptr;

MainWindow::MainWindow(HINSTANCE hInstance, int width, int height, int showCmd) :
    m_hInstance(hInstance),
    m_width(width),
    m_height(height),
    m_showCmd(showCmd)
{
    MainWindowInstance = this;
}

MainWindow::~MainWindow()
{
    delete& Input::GetInstance();
}

HRESULT MainWindow::InitializeWindow()
{
    // Register the window class
    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindow::WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc))
    {
        DWORD dwError = GetLastError();
        if (dwError != ERROR_CLASS_ALREADY_EXISTS)
            return HRESULT_FROM_WIN32(dwError);
    }

    RECT clientRect = {};
    SetRect(&clientRect, 0, 0, m_width, m_height);
    AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, false);

    // Create the window
    m_hWnd = CreateWindow(
        CLASS_NAME,          // Window class name
        L"EricEngine",     // Window text
        WS_OVERLAPPEDWINDOW, // Window style (WS_OVERLAPPEDWINDOW gives a title bar, a border, a system menu, and minimize/maximize buttons)
        CW_USEDEFAULT,       // X position
        CW_USEDEFAULT,       // Y position
        clientRect.right - clientRect.left, // Width
        clientRect.bottom - clientRect.top, // Height
        NULL,                // Parent window (NULL because this is a top-level window)
        NULL,                // Menu (NULL for no menu)
        m_hInstance,         // Instance handle
        NULL);               // Additional application data (arbitrary data of type void*)

    if (m_hWnd == NULL)
    {
        DWORD dwError = GetLastError();
        return HRESULT_FROM_WIN32(dwError);
    }

    ShowWindow(m_hWnd, m_showCmd);

    Input::GetInstance().Initialize(m_hWnd);

    return S_OK;
}

LRESULT MainWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return MainWindowInstance->HandleMessage(hWnd, uMsg, wParam, lParam);
}

LRESULT MainWindow::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CHAR:
    {
        ImGui::GetIO().AddInputCharacter((char)wParam);
        return 0;
    }

    case WM_CLOSE:
    {
        HMENU hMenu;
        hMenu = GetMenu(hWnd);
        if (hMenu != NULL)
        {
            DestroyMenu(hMenu);
        }
        DestroyWindow(hWnd);
        UnregisterClass(
            CLASS_NAME,
            m_hInstance
        );
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
