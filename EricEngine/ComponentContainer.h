#pragma once

#include "Transform.h"
#include "Mesh.h"
#include <unordered_map>

#define INVALID_COMPONENT 0

class ComponentContainer
{
private:
    static ComponentContainer* instance;

public:
    // ComponentContainer Singleton
    static ComponentContainer& GetInstance()
    {
        if (!instance) instance = new ComponentContainer();
        return *instance;
    }

    std::unordered_map<UINT32, Transform> transforms;
    std::unordered_map<UINT32, Mesh> meshes;

    UINT32 nextTransformID = 1;
    UINT32 nextMeshID = 1;

    
    // Creates a new component of type T and returns its component id (i.e. its key in the map)
    template <typename T>
    UINT32 AddComponent(T& component);

    template <>
    UINT32 AddComponent<Transform>(Transform& component)
    {
        UINT32 id = nextTransformID;
        transforms[id] = component;
        nextTransformID++;
        return id;
    }

    template <>
    UINT32 AddComponent<Mesh>(Mesh& component)
    {
        UINT32 id = nextMeshID;
        meshes[id] = component;
        nextMeshID++;
        return id;
    }

    
    // Removes a component of type T from the ComponentContainer. id is the component's key in the map
    template <typename T>
    void RemoveComponent(UINT32 id);

    template<>
    void RemoveComponent<Transform>(UINT32 id) { transforms.erase(id); }

    template<>
    void RemoveComponent<Mesh>(UINT32 id) { meshes.erase(id); }
};