#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "D3DResources.h"
#include <memory>
#include "EntityManager.h"
#include <DirectXMath.h>
#include <string>

#pragma comment (lib, "d3d11.lib")

struct Mesh : ECS::Component
{
    DirectX::XMFLOAT3 boundingMax;
    DirectX::XMFLOAT3 boundingMin;
    int indices;
    std::string name;

    Mesh() : indices(0), boundingMax(), boundingMin(), name("")
    {
    }

    Mesh(const Mesh& other) : indices(other.indices), boundingMax(other.boundingMax), boundingMin(other.boundingMin), name(other.name)
    {
    }

    virtual ~Mesh() {}

    static int id;
    virtual int ID()
    {
        return id;
    }
};