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

    float closestHit = -1;
    int closestEntity = INVALID_ENTITY;
    for (auto& e : entitiesWithMesh)
    {
        auto mesh = em.GetComponent<Mesh>(e);
        auto transform = em.GetComponent<Transform>(e);
        em.GetComponent <Material>(e)->tint = { 1, 1, 1 };
        // Transform min and max to model position
        auto min = mesh->boundingMin;
        auto max = mesh->boundingMax;
        XMVECTOR minMath = XMLoadFloat3(&min);
        XMVECTOR maxMath = XMLoadFloat3(&max);
        XMFLOAT3 pos = transform->GetPosition();
        XMFLOAT3 scale = transform->GetScale();
        // todo: The bounding box won't be perfectly accurate, but that's okay for now.
        XMMATRIX translationMat = XMMatrixScaling(scale.x, scale.y, scale.z);
        translationMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);
        minMath = XMVector3Transform(minMath, translationMat);
        maxMath = XMVector3Transform(maxMath, translationMat);
        XMStoreFloat3(&min, minMath);
        XMStoreFloat3(&max, maxMath);

        t[0] = (min.x - origin.x) / direction.x;
        t[1] = (max.x - origin.x) / direction.x;
        t[2] = (min.y - origin.y) / direction.y;
        t[3] = (max.y - origin.y) / direction.y;
        t[4] = (min.z - origin.z) / direction.z;
        t[5] = (max.z - origin.z) / direction.z;

        mint = (std::max)((std::max)((std::min)(t[0], t[1]), (std::min)(t[2], t[3])), (std::min)(t[4], t[5]));
        maxt = (std::min)((std::min)((std::max)(t[0], t[1]), (std::max)(t[2], t[3])), (std::max)(t[4], t[5]));

        // no intersection
        if (mint > maxt) continue;

        // Origin is inside of the object
        float closestT = mint;
        //if (mint < 0.0f) closestT = maxt;

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
