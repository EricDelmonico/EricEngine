#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include "MainWindow.h"
#include <memory>
#include "D3DResources.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "Input.h"
#include "Transform.h"
#include "Entity.h"
#include "Camera.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    MainWindow mw(hInstance, 1280, 720, nCmdShow);

    // Create and initialize window
    HRESULT hr = S_OK;
    hr = mw.InitializeWindow();
    if (FAILED(hr)) return hr;

    // Create and initialize D3D11
    std::shared_ptr<D3DResources> d3dResources = std::make_shared<D3DResources>(1280, 720);
    d3dResources->Initialize(mw.GetWindow());

    // Create asset manager
    std::unique_ptr<AssetManager> assetManager = std::make_unique<AssetManager>(d3dResources);

    // Create Camera and renderer
    std::shared_ptr<Camera> camera = std::make_shared<Camera>(
        0,              // x
        0,              // y
        0,              // z
        1,              // moveSpeed
        1,              // lookSpeed
        3.14f / 3.0f,             // FOV
        16.0f / 9.0f);  // aspectRatio
    std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(d3dResources, camera, assetManager.get());

    // Create a basic cube entity
    std::shared_ptr<Mesh> cubeMesh = assetManager->LoadMesh("cube.obj");
    Transform transform = Transform();
    std::shared_ptr<Entity> cube = std::make_shared<Entity>(transform, cubeMesh.get());
    std::vector<std::shared_ptr<Entity>> entities;
    entities.push_back(cube);

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
            Input::GetInstance().Update();

            camera->Update(.004f);

            renderer->Render(entities);

            Input::GetInstance().EndOfFrame();
        }
    }

    return 0;
}