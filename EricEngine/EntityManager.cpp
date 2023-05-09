#include "EntityManager.h"

using namespace ECS;

EntityManager* EntityManager::instance;
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
    components.resize(numComponentTypes);
    int max = EntityManager::numComponentTypes;
    for (int i = 0; i < max; i++)
    {
        components[i].resize(MAX_ENTITIES);
        for (int j = 0; j < MAX_ENTITIES; j++)
        {
            components[i][j] = new Component();
        }
    }

    // Allocate space for index vectors
    componentEntityIDs.resize(EntityManager::numComponentTypes);
}

ECS::EntityManager::~EntityManager()
{
    DeregisterAllEntities();

    for (int cid = 0; cid < numComponentTypes; cid++)
    {
        for (int eid = 0; eid < MAX_ENTITIES; eid++)
        {
            if (components[cid][eid]->ID() != MESH_ID) delete components[cid][eid];
        }
    }
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
        auto component = components[i][id];
        if (component == nullptr) continue;

        if (component->ID() != INVALID_COMPONENT)
        {
            auto& entitiesVec = componentEntityIDs[i];
            auto it = std::find(entitiesVec.begin(), entitiesVec.end(), id);
            if (it != entitiesVec.end())
                entitiesVec.erase(it);
            if (components[i][id]->ID() != MESH_ID) delete components[i][id];
            components[i][id] = new Component();
        }
    }

    entityCount--;
}

void ECS::EntityManager::DeregisterAllEntities()
{
    for (int i = 0; i < MAX_ENTITIES; i++) 
    {
        // Deregister each entity that exists
        if (entities[i]) DeregisterEntity(i);
    }
}

void ECS::EntityManager::AddComponent(int componentID, int entityID, Component* component)
{
    // Only add components to existing entities
    if (!entities[entityID]) return;
    // Don't allow double adding of components
    if (components[componentID][entityID]->ID() != INVALID_COMPONENT) return;

    // Delete empty component
    if (components[componentID][entityID]->ID() != MESH_ID) delete components[componentID][entityID];

    components[componentID][entityID] = component;

    // We've added a component here, need to keep track of it
    auto& entityIDs = componentEntityIDs[componentID];
    entityIDs.push_back(entityID);
    std::sort(entityIDs.begin(), entityIDs.end());
}

bool ECS::EntityManager::EntityHasComponent(int componentID, int entityID)
{
    // If this is not true, there is no component there
    return components[componentID][entityID]->ID() == componentID;
}