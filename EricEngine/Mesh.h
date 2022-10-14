#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "D3DResources.h"
#include <memory>
#include "EntityManager.h"

#pragma comment (lib, "d3d11.lib")

struct Mesh : ECS::Component
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    int indices;

    Mesh() : vertexBuffer(nullptr), indexBuffer(nullptr), indices(0)
    {
    }

    Mesh(const Mesh& other) : vertexBuffer(other.vertexBuffer), indexBuffer(other.indexBuffer), indices(other.indices)
    {
    }

    static int id;
    virtual int ID()
    {
        return id;
    }
};