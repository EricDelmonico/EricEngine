#ifndef UNICODE
#define UNICODE
#endif

#include "EntityManager.h"
#include "Mesh.h"
#include "Transform.h"
#include <Windows.h>
#include "MainWindow.h"
#include <memory>
#include "D3DResources.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "Input.h"
#include "Camera.h"
#include "Material.h"
#include "Light.h"
#include "SceneLoader.h"

// Check for memory leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

using namespace ECS;

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
    MainWindow mw(hInstance, 1600, 900, nCmdShow);

    // Create and initialize window
    HRESULT hr = S_OK;
    hr = mw.InitializeWindow();
    if (FAILED(hr)) return hr;

    // Assign an id to all component types
    EntityManager::RegisterNewComponentType<Mesh>();
    EntityManager::RegisterNewComponentType<Transform>();
    EntityManager::RegisterNewComponentType<Material>();
    EntityManager::RegisterNewComponentType<Camera>();
    EntityManager::RegisterNewComponentType<Light>();

    // Create and initialize D3D11
    std::shared_ptr<D3DResources> d3dResources = std::make_shared<D3DResources>(1280, 720);
    d3dResources->Initialize(mw.GetWindow());

    // Create asset manager
    AssetManager* assetManager = new AssetManager(d3dResources);

    // Create scene loader
    SceneLoader* sceneLoader = new SceneLoader(assetManager);

    // Create Camera and renderer
    std::shared_ptr<Camera> camera = std::make_shared<Camera>(
        0,              // x
        20,              // y
        30,              // z
        10,              // moveSpeed
        1,              // lookSpeed
        3.14f / 3.0f,   // FOV
        16.0f / 9.0f);  // aspectRatio
    std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(d3dResources);

    EntityManager* em = &EntityManager::GetInstance();

    // Add render camera
    {
        int e = em->RegisterNewEntity();
        em->AddComponent(e, camera.get());
    }

    sceneLoader->LoadScene("Test.scene");

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

            if (Input::GetInstance().KeyPress('U')) sceneLoader->SaveScene("Test2.scene");
            if (Input::GetInstance().KeyPress('P')) sceneLoader->LoadScene("Test2.scene");

            camera->Update(.004f);

            renderer->Render();

            Input::GetInstance().EndOfFrame();
        }
    }

    delete em;
    delete assetManager;
    delete sceneLoader;
}