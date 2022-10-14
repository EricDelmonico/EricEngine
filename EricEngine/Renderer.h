#pragma once

#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <wrl/client.h>
#include "D3DResources.h"
#include "AssetManager.h"
#include "Mesh.h"
#include "ExternalShaderData.h"
#include "Camera.h"
#include "EntityManager.h"

#pragma comment (lib, "d3d11.lib")

class Renderer
{
public:
    Renderer(std::shared_ptr<D3DResources> d3dResources, std::shared_ptr<Camera> camera, AssetManager* assetManager);

    void Render(ECS::EntityManager* em);

private:
    std::shared_ptr<D3DResources> m_d3dResources;
    AssetManager* m_assetManager;

    std::shared_ptr<Camera> m_camera;
};

