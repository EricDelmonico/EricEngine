#include "Raycasting.h"
#include "EntityManager.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "RaycastObject.h"
#include "Input.h"
#include <DirectXMath.h>

using namespace DirectX;

void Raycasting::Update(float dt)
{
    auto& em = ECS::EntityManager::GetInstance();

    // Get all meshes in the scene
    auto entitiesWithMesh = em.GetEntitiesWithComponents<Mesh, Transform, Material, RaycastObject>();

    // Camera will be the source of the raycast
    auto entitiesWithCamera = em.GetEntitiesWithComponents<Camera, Transform>();
    if (entitiesWithCamera.size() <= 0) return;
    Transform* cam = em.GetComponent<Transform>(entitiesWithCamera[0]);

    // Test each of the six faces of each mesh's bounding box
    float t[] = { 0, 0, 0, 0, 0, 0 };
    // If mint < 0 && maxt < 0, there is no intersection, so default to -1
    float mint = -1;
    float maxt = -1;

    XMFLOAT3 origin = cam->position;
    XMFLOAT3 direction = cam->forward;

    float closestHit = INFINITY;
    int closestEntity = INVALID_ENTITY;
    for (auto& e : entitiesWithMesh)
    {
        auto mesh = em.GetComponent<Mesh>(e);
        auto transform = em.GetComponent<Transform>(e);
        em.GetComponent<Material>(e)->tint = { 1, 1, 1 };
        auto min = mesh->boundingMin;
        auto max = mesh->boundingMax;

        // Transform ray to model space
        XMFLOAT3 localOrigin;
        XMFLOAT3 localDirection;
        {
            XMVECTOR start;
            XMVECTOR directionMath;
        
            auto pos = transform->position;
            auto scale = transform->scale;
            auto pitchYawRoll = transform->pitchYawRoll;

            auto worldMat = transform->worldMatrix;
            XMMATRIX invMat = XMMatrixInverse(0, XMLoadFloat4x4(&worldMat));

            // Apply in reverse order
            XMMATRIX backwardsWorldMat = invMat;

            // Transform ray to local space
            start = XMLoadFloat3(&origin);
            directionMath = XMLoadFloat3(&direction);
            XMVECTOR end = start + directionMath;

            XMVECTOR newOrigin = XMVector3Transform(start, invMat);
            XMVECTOR newEnd = XMVector3Transform(end, invMat);
            directionMath = newEnd - newOrigin;
            XMVECTOR newDir = XMVector3Normalize(directionMath);
            
            XMStoreFloat3(&localOrigin, newOrigin);
            XMStoreFloat3(&localDirection, newDir);
        }

        XMFLOAT3 dirFrac = localDirection;
        dirFrac.x = -1.0 * dirFrac.x;
        dirFrac.y = -1.0 * dirFrac.y;
        dirFrac.z = -1.0 * dirFrac.z;

        t[0] = (min.x - localOrigin.x) / dirFrac.x;
        t[1] = (max.x - localOrigin.x) / dirFrac.x;
        t[2] = (min.y - localOrigin.y) / dirFrac.y;
        t[3] = (max.y - localOrigin.y) / dirFrac.y;
        t[4] = (min.z - localOrigin.z) / dirFrac.z;
        t[5] = (max.z - localOrigin.z) / dirFrac.z;

        mint = (std::max)((std::max)((std::min)(t[0], t[1]), (std::min)(t[2], t[3])), (std::min)(t[4], t[5]));
        maxt = (std::min)((std::min)((std::max)(t[0], t[1]), (std::max)(t[2], t[3])), (std::max)(t[4], t[5]));

        if (maxt < 0) continue;

        // no intersection
        if (mint > maxt) continue;

        // Origin is inside of the object
        float closestT = mint;
        if (mint < 0.0f) closestT = maxt;

        if (closestHit < 0.0f || closestT < closestHit)
        {
            closestHit = closestT;
            closestEntity = e;
        }
    }

    // No intersection
    if (closestEntity == INVALID_ENTITY) return;

#if _DEBUG
    Material* mat = em.GetComponent<Material>(closestEntity);
    mat->tint = { .5f, 0.2f, 0.2f };
#else
    // If we click on the closest entity, destroy it
    if (Input::GetInstance().MouseLeftPress())
    {
        em.DeregisterEntity(closestEntity);
    }
#endif
}
