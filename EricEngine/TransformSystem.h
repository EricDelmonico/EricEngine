#pragma once

#include "Transform.h"

class TransformSystem
{
public:
    void Update(float dt);
    TransformSystem();

private:
    void CalculateUp(Transform* transform);
    void CalculateRight(Transform* transform);
    void CalculateForward(Transform* transform);
    void UpdateMatrices(Transform* transform);

public:
    static void MoveAbsolute(Transform* transform, float x, float y, float z);
    static void MoveRelative(Transform* transform, float x, float y, float z);
    static void Rotate(Transform* transform, float pitch, float yaw, float roll);

    static void SetPitchYawRoll(Transform* transform, float pitch, float yaw, float roll);
    static void SetScale(Transform* transform, float x, float y, float z);
    static void SetPosition(Transform* transform, float x, float y, float z);
};