#include "Raycasting.h"
#include "EntityManager.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include <DirectXMath.h>

using namespace DirectX;

void Raycasting::Update(float dt)
{
    auto& em = ECS::EntityManager::GetInstance();

    // Get all meshes in the scene
    auto entitiesWithMesh = em.GetEntitiesWithComponents<Mesh, Transform, Material>();

    // Camera will be the source of the raycast
    auto entitiesWithCamera = em.GetEntitiesWithComponents<Camera>();
    if (entitiesWithCamera.size() <= 0) return;
    Camera* cam = em.GetComponent<Camera>(entitiesWithCamera[0]);

    // Test each of the six faces of each mesh's bounding box
    float t[] = { 0, 0, 0, 0, 0, 0 };
    // If mint < 0 && maxt < 0, there is no intersection, so default to -1
    float mint = -1;
    float maxt = -1;

    XMFLOAT3 origin = cam->GetTransform()->GetPosition();
    XMFLOAT3 direction = cam->GetTransform()->GetForward();

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
            XMVECTOR originMath;
            XMVECTOR directionMath;
        
            auto pos = transform->position;
            auto scale = transform->scale;
            auto pitchYawRoll = transform->pitchYawRoll;

            XMMATRIX transMat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z);
            XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
            auto rotMatDeterminant = XMMatrixDeterminant(rotMat);
            rotMat = XMMatrixInverse(&rotMatDeterminant, rotMat);
            XMMATRIX scaleMat = XMMatrixScaling(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);

            auto worldMat = transform->GetWorldMatrix();
            XMMATRIX invMat = XMMatrixInverse(0, XMLoadFloat4x4(&worldMat));

            // Apply in reverse order
            XMMATRIX backwardsWorldMat = invMat;// transMat * rotMat * scaleMAt;

            // Transform ray to local space
            originMath = XMLoadFloat3(&origin);
            directionMath = XMLoadFloat3(&direction);
            directionMath = directionMath + originMath;

            originMath = XMVector3Transform(originMath, backwardsWorldMat);
            directionMath = XMVector3Transform(directionMath, backwardsWorldMat);
            directionMath = originMath - directionMath;
            
            XMStoreFloat3(&localOrigin, originMath);
            XMStoreFloat3(&localDirection, directionMath);
        }

        XMFLOAT3 dirFrac = localDirection;
        dirFrac.x = 1.0f / dirFrac.x;
        dirFrac.y = 1.0f / dirFrac.y;
        dirFrac.z = 1.0f / dirFrac.z;

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

    Material* mat = em.GetComponent<Material>(closestEntity);
    mat->tint = { 1, 0.2f, 0.2f };
}
