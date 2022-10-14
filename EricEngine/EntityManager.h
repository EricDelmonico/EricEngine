#pragma once

// Heavily inspired by EntityFu, written by Nathanael Weiss -- https://github.com/NatWeiss/EntityFu

#include <vector>
#include <algorithm>

#define MAX_ENTITIES 4096

#define INVALID_ENTITY -1
#define FIRST_VALID_ENTITY INVALID_ENTITY+1

#define INVALID_COMPONENT -1

namespace ECS 
{
    struct Component
    {
        virtual int ID() { return INVALID_COMPONENT; }
    };

    class EntityManager
    {
    private:
        static EntityManager* instance;

        bool entities[MAX_ENTITIES];
        // Array of component arrays. Accessed like components[componentID][entityID], so each
        // component has a contiguous block of memory
        Component*** components;

        // Find an empty id slot to add an entity to.
        int GetID();

        // Initializes this entity manager
        EntityManager();

    public:
        ~EntityManager();

        static int numComponentTypes;

        // Keep track of where valid components actually are so we
        // can iterate through them
        std::vector<int>* componentEntityIDs;

        // ComponentContainer Singleton
        static EntityManager& GetInstance();

        // Makes a new entity and returns its id
        int RegisterNewEntity();
        // Clears out an entity's space
        void DeregisterEntity(int id);
        

        // Adds component of type ComponentType to the entity. componentID is used
        // to stick the component into the entity structs components array
        template <class ComponentType>
        void AddComponent(int id, ComponentType* component);

        // Removes component of type ComponentType from the entity with the given entityID
        template <class ComponentType>
        void RemoveComponent(int entityID);

        template <class ComponentType>
        ComponentType* GetComponent(int entityID);
    };

    template<class ComponentType>
    inline void EntityManager::AddComponent(int id, ComponentType* component)
    {
        // Can't add a component to an invalid entity
        if (!entities[id]) return;

        int componentID = component->ID();
        components[componentID][id] = component;

        // We've added a component here, need to keep track of it
        auto& entityIDs = componentEntityIDs[componentID];
        entityIDs.push_back(id);
        std::sort(entityIDs.begin(), entityIDs.end());
    }
    template<class ComponentType>
    inline void EntityManager::RemoveComponent(int entityID)
    {
        int componentID = ComponentType::ID();
        // Set to an empty component, will have an ID of INVALID_COMPONENT which is useful
        delete components[componentID][entityID];
        components[componentID][entityID] = nullptr;

        auto& entitiesVec = componentEntityIDs[componentID];
        auto it = std::find(entitiesVec.begin(), entitiesVec.end(), entityID);
        if (it != entitiesVec.end())
            entitiesVec.erase(it);
    }
    template<class ComponentType>
    inline ComponentType* EntityManager::GetComponent(int entityID)
    {
        int componentID = ComponentType::id;
        return static_cast<ComponentType*>(components[componentID][entityID]);
    }
}