#pragma once

// Heavily inspired by EntityFu, written by Nathanael Weiss -- https://github.com/NatWeiss/EntityFu

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <tuple>
#include <iterator>
#include <boost/mp11.hpp>
#include <boost/range/algorithm/set_algorithm.hpp>

#define MAX_ENTITIES 4096

#define INVALID_ENTITY -1
#define FIRST_VALID_ENTITY INVALID_ENTITY+1

#define INVALID_COMPONENT -1

namespace ECS
{
    struct Component
    {
        virtual int ID() { return INVALID_COMPONENT; }
        virtual int Size() { throw; }
    };

    class EntityManager
    {
    private:
        std::vector<std::vector<int>*> vectorsToIntersect;
        static std::unordered_map<int, int> componentTypeSizes;

        static EntityManager* instance;

        // Find an empty id slot to add an entity to.
        int GetID();

        // Array of component arrays. Accessed like components[componentID][entityID], so each
        // component has a contiguous block of memory
        std::vector<std::vector<Component*>> components;

        // Initializes this entity manager
        EntityManager();

    public:
        ~EntityManager();

        static int numComponentTypes;

        bool entities[MAX_ENTITIES];
        int entityCount;

        // Keep track of where valid components actually are so we
        // can iterate through them
        std::vector<int>* componentEntityIDs;

        // ComponentContainer Singleton
        static EntityManager& GetInstance();

        // Makes a new entity and returns its id
        int RegisterNewEntity();
        // Clears out an entity's space
        void DeregisterEntity(int id);

        // Clears out all entities
        void DeregisterAllEntities();

        int GetComponentSizeFromID(int componentID);

        // Adds component of type ComponentType to the entity. componentID is used
        // to stick the component into the entity structs components array
        template <class ComponentType>
        void AddComponent(int id, ComponentType* component);

        // Adds a component by id
        void AddComponent(int componentID, int entityID, Component* component);

        // Removes component of type ComponentType from the entity with the given entityID
        template <class ComponentType>
        void RemoveComponent(int entityID);

        template <class ComponentType>
        ComponentType* GetComponent(int entityID);

        template <class... ComponentTypes>
        std::vector<int> GetEntitiesWithComponents();

        template <class ComponentType>
        static void RegisterNewComponentType();
    };

    template<class ComponentType>
    inline void EntityManager::AddComponent(int id, ComponentType* component)
    {
        AddComponent(ComponentType::id, id, component);
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
        return (ComponentType*)(components[componentID][entityID]);
    }
    template<class... ComponentTypes>
    inline std::vector<int> EntityManager::GetEntitiesWithComponents()
    {
        std::vector<int> intersection;
        std::tuple<ComponentTypes...> types;
        int tupleSize = std::tuple_size<decltype(types)>::value;
        // Need at least two types for an intersection
        if (tupleSize < 2)
        {
            // Get all entities with the component passed in, if one was passed in
            if (tupleSize == 1)
            {
                return componentEntityIDs[std::get<0>(types).ID()];
            }
            // If none were passed in, return an empty vector
            else
            {
                return intersection;
            }
        }

        // Handle set intersection
        vectorsToIntersect.clear();
        boost::mp11::tuple_for_each(types, [&](const auto& type) {
            Component* comp = static_cast<Component*>((void*)(&type));
            int id = comp->ID();
            assert(id != INVALID_COMPONENT); // Type MUST be a component
            vectorsToIntersect.push_back(&componentEntityIDs[id]);
        });

        std::vector<int>& prevVec = *vectorsToIntersect[0];
        for (int i = 1; i < vectorsToIntersect.size(); i++)
        {
            std::vector<int>& nextVec = *vectorsToIntersect[i];
            boost::set_intersection(prevVec, nextVec, std::back_inserter(intersection));
            prevVec = intersection;
            intersection.clear();
        }
        

        return prevVec;
    }

    template <class ComponentType>
    static inline void EntityManager::RegisterNewComponentType()
    {
        ComponentType::id = numComponentTypes++;
        componentTypeSizes.insert({ ComponentType::id, sizeof(ComponentType) });
    }
}