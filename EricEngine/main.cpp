#ifndef UNICODE
#define UNICODE
#endif

#include "EntityManager.h"
#include "Mesh.h"
#include "Transform.h"
#include "MainWindow.h"
#include "D3DResources.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "Input.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "SceneLoader.h"
#include "SceneEditor.h"
#include "CameraControl.h"
#include "Raycasting.h"
#include "RaycastObject.h"
#include "TransformSystem.h"

#include <Windows.h>
#include <memory>
#include <chrono>

// ImGui
#ifdef _DEBUG
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#endif

// Check for memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <boost/algorithm/string.hpp>

const int WIDTH = 1600;
const int HEIGHT = 900;

using namespace ECS;

#ifdef _DEBUG
void InitializeImGui(void* hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsClassic();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);
}

void UpdateImGui(SceneLoader* sceneLoader, char* sceneName, float deltaTime)
{
    auto& input = Input::GetInstance();

    // Take care of input
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = deltaTime;
    io.DisplaySize.x = WIDTH;
    io.DisplaySize.y = HEIGHT;
    io.KeyCtrl = input.KeyDown(VK_CONTROL);
    io.KeyShift = input.KeyDown(VK_SHIFT);
    io.KeyAlt = input.KeyDown(VK_MENU);
    io.MousePos.x = (float)input.GetMouseX();
    io.MousePos.y = (float)input.GetMouseY();
    io.MouseDown[0] = input.MouseLeftDown();
    io.MouseDown[1] = input.MouseRightDown();
    io.MouseDown[2] = input.MouseMiddleDown();
    io.MouseWheel = input.GetMouseWheel();
    input.GetKeyArray(io.KeysDown, 256);
    
    // Styling
    {
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
        style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
        style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        style.GrabRounding = style.FrameRounding = 2.3f;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    input.SetGuiKeyboardCapture(io.WantCaptureKeyboard);
    input.SetGuiMouseCapture(io.WantCaptureMouse);

    ImGui::Begin("SceneLoader");

    std::string sceneStr = sceneName;
    if (ImGui::InputText("Scene Name", sceneName, 128))
    {
        sceneStr = sceneName;
        boost::trim_right(sceneStr);
    }

    if (ImGui::Button("Save Scene"))
    {
        sceneLoader->SaveScene(sceneStr);
    }

    if (Input::GetInstance().KeyDown(VK_CONTROL) && Input::GetInstance().KeyPress('S'))
    {
        sceneLoader->SaveScene(sceneStr);
    }

    if (ImGui::Button("Load Scene"))
    {
        sceneLoader->LoadScene(sceneStr);
    }

    ImGui::End();
}
#endif

HRESULT main(HINSTANCE hInstance, int nCmdShow);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
   HRESULT hr = main(hInstance, nCmdShow);
   if (FAILED(hr))
   {
       return hr;
   }

#ifdef _DEBUG
    _CrtDumpMemoryLeaks();
#endif

    return 0;
}

HRESULT main(HINSTANCE hInstance, int nCmdShow)
{
    MainWindow mw(hInstance, WIDTH, HEIGHT, nCmdShow);

    // Create and initialize window
    HRESULT hr = S_OK;
    hr = mw.InitializeWindow();
    if (FAILED(hr)) return hr;

    // Assign an id to all component types
    // Mesh needs to be registered first......  would like to find a better solution
    EntityManager::RegisterNewComponentType<Mesh>();
    EntityManager::RegisterNewComponentType<Transform>();
    EntityManager::RegisterNewComponentType<Material>();
    EntityManager::RegisterNewComponentType<Camera>();
    EntityManager::RegisterNewComponentType<LightComponent>();
    EntityManager::RegisterNewComponentType<RaycastObject>();

    // Create and initialize D3D11
    std::shared_ptr<D3DResources> d3dResources = std::make_shared<D3DResources>(WIDTH, HEIGHT);
    d3dResources->Initialize(mw.GetWindow());

#ifdef _DEBUG
    InitializeImGui(mw.GetWindow(), d3dResources->GetDevice(), d3dResources->GetContext());
#endif

    // Create asset manager
    AssetManager* assetManager = new AssetManager(d3dResources);
    // Create scene loader
    SceneLoader* sceneLoader = new SceneLoader(assetManager);

#if _DEBUG
    // Create scene editor
    SceneEditor sceneEditor(sceneLoader, assetManager);
#endif

    TransformSystem transformSystem;

    // Create Camera
    Camera* camera = new Camera();
    camera->movementSpeed = 10;
    camera->mouseLookSpeed = 1;
    camera->fieldOfView = 3.14f / 3.0f;
    camera->aspectRatio = 16.0f / 9.0f;
    camera->orthoSize = 2.5f;
    Transform* camTransform = new Transform();
    TransformSystem::SetPosition(camTransform, 0, 20, 30);

    // ---------------- initialize systems ----------------
    std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(d3dResources, assetManager);
    CameraControl camControl = CameraControl(mw.GetWindow(), WIDTH, HEIGHT);
    Raycasting raycasting = Raycasting();
    // ----------------------------------------------------

    EntityManager* em = &EntityManager::GetInstance();

    // Add render camera
    {
        int e = em->RegisterNewEntity();
        em->AddComponent(e, camera);
        em->AddComponent(e, camTransform);
    }

    auto prevFrameTime = std::chrono::high_resolution_clock::now();
    auto thisFrameTime = prevFrameTime;

#ifdef _DEBUG
    char sceneName[128] = "Default.scene";
#else
    // Load scene
    char sceneName[128] = "Default.scene";
    sceneLoader->LoadScene(sceneName);
#endif

    MSG msg = { };
    while (msg.message != WM_QUIT)
    {
        // Check for a message and translate/dispatch if there is one
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            thisFrameTime = std::chrono::high_resolution_clock::now();
            float dt = (float)std::chrono::duration_cast<std::chrono::microseconds>(thisFrameTime - prevFrameTime).count() / 1000000.0f;

            Input::GetInstance().SetGuiKeyboardCapture(false);
            Input::GetInstance().SetGuiMouseCapture(false);

            Input::GetInstance().Update();

#if _DEBUG
            UpdateImGui(sceneLoader, sceneName, dt);
            sceneEditor.Update(dt);
#endif

            // ------------------ update systems ------------------
            transformSystem.Update(dt);
            camControl.Update(dt);
            raycasting.Update(dt);
            renderer->Render();
            // ----------------------------------------------------

            Input::GetInstance().EndOfFrame();

            prevFrameTime = thisFrameTime;
        }
    }

#ifdef _DEBUG
    // ImGui clean up
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#endif

    delete em;
    delete assetManager;
    delete sceneLoader;
}