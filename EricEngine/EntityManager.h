#pragma once
#include "Entity.h"
#include "Transform.h"
#include "Mesh.h"
#include "ComponentContainer.h"

#include <vector>

#define MAX_ENTITIES 16384
#define FIRST_VALID_ENTITY INVALID_ENTITY+1

class EntityManager
{
private:
    static EntityManager* instance;

    ComponentContainer& componentContainer;
    Entity entities[MAX_ENTITIES];
    UINT32 nextID;
    bool outOfRoom;

    // Adds one to nextID, or if we've reached the max amount,
    // or have hit an already filled slot, advance nextID 
    // until we find an empty slot
    void AdvanceNextID()
    {
        nextID++;

        if (nextID >= MAX_ENTITIES || entities[nextID].id != INVALID_ENTITY)
        {
            nextID = INVALID_ENTITY;
            for (UINT32 i = FIRST_VALID_ENTITY; i < MAX_ENTITIES; i++)
            {
                // If we found an empty slot, use it
                if (entities[i].id == INVALID_ENTITY) nextID = i;
            }

            // We didn't find an open slot, so we have no room for additional entities.
            if (nextID == INVALID_ENTITY) outOfRoom = true;
        }
    }

    // Initializes this entity manager
    EntityManager() :
        componentContainer(ComponentContainer::GetInstance()),
        nextID(FIRST_VALID_ENTITY),
        outOfRoom(false),
        entities()
    {
    }

public:
    // ComponentContainer Singleton
    static EntityManager& GetInstance()
    {
        if (!instance) instance = new EntityManager();
        return *instance;
    }

    // Clears a spot for a new entity in entities array. Sets the new 
    // entity's id to its index, then returns a pointer to the Entity.
    Entity* RegisterNewEntity()
    {
        // If we're out of space, don't try to register a new entity
        if (outOfRoom) return nullptr;

        // Empty out the memory in the new slot
        UINT32 id = nextID;
        entities[id] = { };

        // Assign the ID, then advance the nextID forward
        entities[id].id = id;
        AdvanceNextID();

        return &entities[id];
    }

    // Marks a location in the entities array as invalid
    void DeregisterEntity(UINT32 id)
    {
        auto& e = entities[id];
        
        // Check each component type to see if it exists, and if it does, remove it
        // TODO: possibly find a way to not hard code this? Maybe enums?
        if (e.components[TRANSFORM] != INVALID_COMPONENT) RemoveComponent<Transform>(id, TRANSFORM);
        if (e.components[MESH] != INVALID_COMPONENT) RemoveComponent<Mesh>(id, MESH);

        // Move our last entity to the newly available slot
        auto& lastE = entities[nextID - 1];
        if (lastE.id != INVALID_ENTITY)
        {
            // Move the last entity
            entities[id] = lastE;
            // Set the now vacant slot to be actually vacant
            entities[nextID - 1].id = INVALID_ENTITY;
            // Set id to the new index
            entities[id].id = id;
        }

        // Something was removed
        nextID--;

        outOfRoom = false;
    }

    // Adds component of type T to the entity. componentID is used
    // to stick the component into the entity structs components array
    template <typename T>
    void AddComponent(UINT32 id, T& component, UINT8 componentID)
    {
        // Can't add a component to an invalid entity
        assert(entities[id].id != INVALID_ENTITY);

        auto& entity = entities[id];
        entity.components[componentID] = componentContainer.AddComponent<T>(component);

        assert(entity.components[componentID] != INVALID_COMPONENT);
    }

    // Removes component of type T from the entity with the given entityID
    template <typename T>
    void RemoveComponent(UINT32 entityID, UINT8 componentID)
    {
        componentContainer.RemoveComponent<T>(entities[entityID].components[componentID]);
        entities[entityID].components[componentID] = INVALID_COMPONENT;
    }
};