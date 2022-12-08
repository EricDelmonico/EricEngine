#pragma once
#include "EntityManager.h"

class RaycastObject : public ECS::Component
{
public:
    static int id;
    virtual int ID()
    {
        return id;
    }
};

