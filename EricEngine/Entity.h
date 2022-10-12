#pragma once
#include "Transform.h"
#include "Mesh.h"

#define MAX_COMPONENTS 15
#define INVALID_ENTITY 0

// Index in the components array where these components' indices are stored
#define TRANSFORM 0
#define MESH 1

struct Entity
{
    // Array of component map keys, or 0 if this entity does not have the given component
    UINT32 components[MAX_COMPONENTS];

    // Index of this object inn the EntityManager.
    // An ID of 0 indicates a "dead" entity.
    UINT32 id;
};

