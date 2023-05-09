#include "TransformSystem.h"
#include "EntityManager.h"
#include <DirectXMath.h>

using namespace ECS;
using namespace DirectX;

TransformSystem::TransformSystem()
{
}

void TransformSystem::CalculateUp(Transform* transform)
{
    // Take the world up vector and rotate it by out own rotation values
    XMVECTOR localUp = XMVector3Rotate(
        XMVectorSet(0, 1, 0, 0),
        XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&transform->pitchYawRoll)));

    XMStoreFloat3(&transform->up, localUp);
}

void TransformSystem::CalculateRight(Transform* transform)
{
    // Take the world right vector and rotate it by out own rotation values
    XMVECTOR localRight = XMVector3Rotate(
        XMVectorSet(1, 0, 0, 0),
        XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&transform->pitchYawRoll)));

    XMStoreFloat3(&transform->right, localRight);
}

void TransformSystem::CalculateForward(Transform* transform)
{
    // Take the world forward vector and rotate it by out own rotation values
    XMVECTOR localForward = XMVector3Rotate(
        XMVectorSet(0, 0, 1, 0),
        XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&transform->pitchYawRoll)));

    XMStoreFloat3(&transform->forward, localForward);
}

void TransformSystem::UpdateMatrices(Transform* transform)
{
    // update the matrices by creating the individual transformation 
    // matrices and combining
    XMMATRIX transMat = XMMatrixTranslation(
        transform->position.x,
        transform->position.y,
        transform->position.z);
    XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(
        transform->pitchYawRoll.x,
        transform->pitchYawRoll.y,
        transform->pitchYawRoll.z);
    XMMATRIX scaleMat = XMMatrixScaling(
        transform->scale.x,
        transform->scale.y,
        transform->scale.z);

    // Combine into a single matrix that represents all transformations
    // and store the results
    XMMATRIX worldMat = scaleMat * rotMat * transMat; // SRT
    XMStoreFloat4x4(&transform->worldMatrix, worldMat);

    // While we're at it, create the inverse transpose matrix
    XMStoreFloat4x4(
        &transform->worldInverseTransposeMatrix,
        XMMatrixInverse(0, XMMatrixTranspose(worldMat))
    );


    // Clean once again!
    transform->matricesDirty = false;
}

void TransformSystem::Update(float dt)
{
    EntityManager& em = EntityManager::GetInstance();
    auto allTransforms = em.GetAllComponentsOfType<Transform>();

    for (int i = 0; i < allTransforms.size(); i++)
    {
        auto component = allTransforms[i];
        if (component->ID() == INVALID_COMPONENT) continue;
        auto t = (Transform*)component;
        if (!t->matricesDirty) continue;

        UpdateMatrices(t);

        CalculateUp(t);
        CalculateRight(t);
        CalculateForward(t);
    }
}

void TransformSystem::MoveAbsolute(Transform* transform, float x, float y, float z)
{
    auto& pos = transform->position;
    pos.x += x;
    pos.y += y;
    pos.z += z;
    transform->matricesDirty = true;
}

void TransformSystem::MoveRelative(Transform* transform, float x, float y, float z)
{
    XMVECTOR rotatedVector = XMVector3Rotate(
        XMVectorSet(x, y, z, 0),
        XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&transform->pitchYawRoll)));

    // Overwrite position with current pos + rotated movement
    XMStoreFloat3(
        &transform->position,
        XMLoadFloat3(&transform->position) + rotatedVector);
    transform->matricesDirty = true;
}

void TransformSystem::Rotate(Transform* transform, float pitch, float yaw, float roll)
{
    auto& pyr = transform->pitchYawRoll;
    pyr.x += pitch;
    pyr.y += yaw;
    pyr.z += roll;
    transform->matricesDirty = true;
}

void TransformSystem::SetPitchYawRoll(Transform* transform, float pitch, float yaw, float roll)
{
    auto& pyr = transform->pitchYawRoll;
    pyr.x = pitch;
    pyr.y = yaw;
    pyr.z = roll;
    transform->matricesDirty = true;
}

void TransformSystem::SetScale(Transform* transform, float x, float y, float z)
{
    auto& scale = transform->scale;
    scale.x = x;
    scale.y = y;
    scale.z = z;
    transform->matricesDirty = true;
}

void TransformSystem::SetPosition(Transform* transform, float x, float y, float z)
{
    auto& pos = transform->position;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    transform->matricesDirty = true;
}