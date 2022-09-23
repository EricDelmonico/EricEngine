#pragma once
#include "Transform.h"
#include "Mesh.h"

class Entity
{
public:
    Entity(Transform transform, Mesh mesh);

    Transform* GetTransform() { return &m_transform; }
    Mesh* GetMesh() { return &m_mesh; }

private:
    Transform m_transform;
    Mesh m_mesh;
};

