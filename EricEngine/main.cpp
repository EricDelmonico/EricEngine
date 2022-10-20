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
    Mesh::id = EntityManager::numComponentTypes++;
    Transform::id = EntityManager::numComponentTypes++;
    Material::id = EntityManager::numComponentTypes++;
    Camera::id = EntityManager::numComponentTypes++;
    Light::id = EntityManager::numComponentTypes++;

    // Create and initialize D3D11
    std::shared_ptr<D3DResources> d3dResources = std::make_shared<D3DResources>(1280, 720);
    d3dResources->Initialize(mw.GetWindow());

    // Create asset manager
    AssetManager* assetManager = new AssetManager(d3dResources);

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

    std::vector<std::shared_ptr<Transform>> transforms;
    std::shared_ptr<Material> cameraMaterial;
    cameraMaterial = std::make_shared<Material>();
    cameraMaterial->albedo = assetManager->GetTexture(L"1cam_albedo.png");
    cameraMaterial->metalness = assetManager->GetTexture(L"1cam_metalness.png");
    cameraMaterial->normals = assetManager->GetTexture(L"1cam_normals.png");
    cameraMaterial->roughness = assetManager->GetTexture(L"1cam_roughness.png");
    cameraMaterial->ao = assetManager->GetTexture(L"white_roughness.png");
    cameraMaterial->samplerState = assetManager->GetSamplerState();
    cameraMaterial->pixelShader = assetManager->GetPixelShader(L"PixelShader").get();
    cameraMaterial->vertexShader = assetManager->GetVertexShader(L"VertexShader").get();

    std::shared_ptr<Material> handMaterial;
    handMaterial = std::make_shared<Material>();
    handMaterial->albedo = assetManager->GetTexture(L"hand_albedo.jpg");
    handMaterial->metalness = assetManager->GetTexture(L"gray_roughness.png"); // Not metal
    handMaterial->normals = assetManager->GetTexture(L"hand_normals.png");
    handMaterial->roughness = assetManager->GetTexture(L"hand_roughness.jpg");
    handMaterial->ao = assetManager->GetTexture(L"hand_AO.jpg");
    handMaterial->samplerState = assetManager->GetSamplerState();
    handMaterial->pixelShader = assetManager->GetPixelShader(L"PixelShader").get();
    handMaterial->vertexShader = assetManager->GetVertexShader(L"VertexShader").get();

    std::shared_ptr<Material> tableMaterial;
    tableMaterial = std::make_shared<Material>();
    tableMaterial->albedo = assetManager->GetTexture(L"table_albedo.png");
    tableMaterial->metalness = assetManager->GetTexture(L"table_metalness.png");
    tableMaterial->normals = assetManager->GetTexture(L"table_normals.png");
    tableMaterial->roughness = assetManager->GetTexture(L"table_roughness.png");
    tableMaterial->ao = assetManager->GetTexture(L"table_ao.png");
    tableMaterial->samplerState = assetManager->GetSamplerState();
    tableMaterial->pixelShader = assetManager->GetPixelShader(L"PixelShader").get();
    tableMaterial->vertexShader = assetManager->GetVertexShader(L"VertexShader").get();

    // Add render camera
    {
        int e = em->RegisterNewEntity();
        em->AddComponent(e, camera.get());
    }

    // Add directional light
    std::shared_ptr<Light> light = std::make_shared<Light>();
    light->color = { 1, 1, 1 };
    light->dir = { 0, -1, 1 };
    light->intensity = 2.0f;
    {
        int e = em->RegisterNewEntity();
        em->AddComponent(e, light.get());
    }

    // Add camera model
    {
        int e = em->RegisterNewEntity();
        std::shared_ptr<Transform> t = std::make_shared<Transform>();
        transforms.push_back(t);
        t->SetPosition(5.0f, 15.6f, 12.0f);
        t->SetScale(.5f, .5f, .5f);
        t->SetPitchYawRoll(0, 3.14f / 2, 0);
        em->AddComponent<Transform>(e, t.get());

        Mesh* mesh = assetManager->GetMesh("cam.obj");
        em->AddComponent<Mesh>(e, mesh);

        em->AddComponent<Material>(e, cameraMaterial.get());
    }

    // Add hands
    {
        int e = em->RegisterNewEntity();
        std::shared_ptr<Transform> t = std::make_shared<Transform>();
        transforms.push_back(t);
        t->SetPosition(-5.0f, 15.6f, -12.0f);
        t->SetScale(4, 4, 4);
        t->SetPitchYawRoll(0, 3.14f / 3, 0);
        em->AddComponent<Transform>(e, t.get());

        Mesh* mesh = assetManager->GetMesh("hand.obj");
        em->AddComponent<Mesh>(e, mesh);

        em->AddComponent<Material>(e, handMaterial.get());
    }
    {
        int e = em->RegisterNewEntity();
        std::shared_ptr<Transform> t = std::make_shared<Transform>();
        transforms.push_back(t);
        t->SetScale(4, 4, 4);
        t->SetPitchYawRoll(0, 3.14f / 2, 0);
        t->SetPosition(-5.0f, 15.6f, -6.0f);
        em->AddComponent<Transform>(e, t.get());

        Mesh* mesh = assetManager->GetMesh("hand.obj");
        em->AddComponent<Mesh>(e, mesh);

        em->AddComponent<Material>(e, handMaterial.get());
    }
    {
        int e = em->RegisterNewEntity();
        std::shared_ptr<Transform> t = std::make_shared<Transform>();
        transforms.push_back(t);
        t->SetScale(4, 4, 4);
        t->SetPitchYawRoll(0, 3 * 3.14f / 4, 0);
        t->SetPosition(-5.0f, 15.6f, 0.0f);
        em->AddComponent<Transform>(e, t.get());

        Mesh* mesh = assetManager->GetMesh("hand.obj");
        em->AddComponent<Mesh>(e, mesh);

        em->AddComponent<Material>(e, handMaterial.get());
    }

    // Add table
    {
        int e = em->RegisterNewEntity();
        std::shared_ptr<Transform> t = std::make_shared<Transform>();
        transforms.push_back(t);
        t->SetPosition(0.0f, 0.0f, 0.0f);
        t->SetScale(0.25f, 0.25f, 0.25f);
        t->SetPitchYawRoll(0, 90, 0);
        em->AddComponent<Transform>(e, t.get());

        Mesh* mesh = assetManager->GetMesh("table.obj");
        em->AddComponent<Mesh>(e, mesh);

        em->AddComponent<Material>(e, tableMaterial.get());
    }

    // Test de-registering an entity
    //em->DeregisterEntity(1);

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

            renderer->Render();

            Input::GetInstance().EndOfFrame();
        }
    }

    delete em;
    delete assetManager;
}