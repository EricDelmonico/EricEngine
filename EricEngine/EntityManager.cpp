#include "EntityManager.h"

using namespace ECS;

EntityManager* EntityManager::instance;
std::unordered_map<int, int> EntityManager::componentTypeSizes;
int EntityManager::numComponentTypes;

int ECS::EntityManager::GetID()
{
    int eid = FIRST_VALID_ENTITY;

    if (eid >= MAX_ENTITIES || entities[eid])
    {
        for (; eid < MAX_ENTITIES; eid++)
        {
            // If we found an empty slot, use it
            if (!entities[eid]) return eid;
        }

        // We didn't find an open slot, so we have no room for additional entities.
        eid = INVALID_ENTITY;
    }

    return eid;
}

ECS::EntityManager::EntityManager() : entities()
{
    entityCount = 0;

    // Allocate component memory
    // Each component has a full array,
    // where every entity has one slot
    components = new Component**[EntityManager::numComponentTypes];
    int max = EntityManager::numComponentTypes;
    for (int i = 0; i < max; i++)
    {
        components[i] = new Component*[MAX_ENTITIES];
        for (int j = 0; j < MAX_ENTITIES; j++)
        {
            components[i][j] = {};
        }
    }

    // Allocate space for index vectors
    componentEntityIDs = new std::vector<int>[EntityManager::numComponentTypes];
}

ECS::EntityManager::~EntityManager()
{
    // Deallocate component arrays
    for (int i = 0; i < EntityManager::numComponentTypes; i++)
    {
        delete[] components[i];
    }
    delete[] components;

    delete[] componentEntityIDs;
}

EntityManager& ECS::EntityManager::GetInstance()
{
    if (!instance) instance = new EntityManager();
    return *instance;
}

int ECS::EntityManager::RegisterNewEntity()
{
    int id = GetID();
    entities[id] = true;
    entityCount++;
    return id;
}

void ECS::EntityManager::DeregisterEntity(int id)
{
    entities[id] = false;
    // Clear out any valid components
    for (int i = 0; i < EntityManager::numComponentTypes; i++)
    {
        if (components[i][id] == nullptr) continue;

        if (components[i][id]->ID() != INVALID_COMPONENT)
        {
            auto& entitiesVec = componentEntityIDs[i];
            auto it = std::find(entitiesVec.begin(), entitiesVec.end(), id);
            if (it != entitiesVec.end())
                entitiesVec.erase(it);
        }
        components[i][id] = nullptr;
    }

    entityCount--;
}

int ECS::EntityManager::GetComponentSizeFromID(int componentID)
{
    if (componentID < 0 || componentID >= numComponentTypes) throw;

    return componentTypeSizes[componentID];
}

void ECS::EntityManager::AddComponent(int componentID, int entityID, Component* component)
{
    // Only add components to existing entities
    if (!entities[entityID]) return;

    components[componentID][entityID] = component;

    // We've added a component here, need to keep track of it
    auto& entityIDs = componentEntityIDs[componentID];
    entityIDs.push_back(entityID);
    std::sort(entityIDs.begin(), entityIDs.end());
}