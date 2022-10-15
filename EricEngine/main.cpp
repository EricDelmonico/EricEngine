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
    MainWindow mw(hInstance, 1280, 720, nCmdShow);

    // Create and initialize window
    HRESULT hr = S_OK;
    hr = mw.InitializeWindow();
    if (FAILED(hr)) return hr;

    // Assign an id to all component types
    Mesh::id = EntityManager::numComponentTypes++;
    Transform::id = EntityManager::numComponentTypes++;
    Material::id = EntityManager::numComponentTypes++;

    // Create and initialize D3D11
    std::shared_ptr<D3DResources> d3dResources = std::make_shared<D3DResources>(1280, 720);
    d3dResources->Initialize(mw.GetWindow());

    // Create asset manager
    AssetManager* assetManager = new AssetManager(d3dResources);

    // Create Camera and renderer
    std::shared_ptr<Camera> camera = std::make_shared<Camera>(
        0,              // x
        0,              // y
        0,              // z
        1,              // moveSpeed
        1,              // lookSpeed
        3.14f / 3.0f,   // FOV
        16.0f / 9.0f);  // aspectRatio
    std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(d3dResources, camera, assetManager);

    EntityManager* em = &EntityManager::GetInstance();

    std::vector<std::shared_ptr<Transform>> transforms;
    std::shared_ptr<Material> rockMaterial;
    rockMaterial = std::make_shared<Material>();
    rockMaterial->albedo = assetManager->GetTexture(L"tripleshotgun_albedo.jpg");
    rockMaterial->metalness = assetManager->GetTexture(L"tripleshotgun_Metallic.jpg");
    rockMaterial->normals = assetManager->GetTexture(L"tripleshotgun_Normals.jpg");
    rockMaterial->roughness = assetManager->GetTexture(L"tripleshotgun_Roughness.jpg");
    rockMaterial->samplerState = assetManager->GetSamplerState();
    for (int i = 0; i < 3969; i++)
    {
        int e = em->RegisterNewEntity();
        std::shared_ptr<Transform> t = std::make_shared<Transform>();
        transforms.push_back(t);
        t->SetPosition((i / 63) * 1.0f, 0.0f, (i % 63) * 1.0f);
        t->SetScale(0.25f, 0.25f, 0.25f);
        em->AddComponent<Transform>(e, t.get());
    
        Mesh* mesh = assetManager->GetMesh("Triple_Barrel_Shotgun.obj");
        em->AddComponent<Mesh>(e, mesh);

        em->AddComponent<Material>(e, rockMaterial.get());
    }

    // Test de-registering an entity
    em->DeregisterEntity(1);

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

            renderer->Render(em);

            Input::GetInstance().EndOfFrame();
        }
    }

    delete em;
    delete assetManager;
}