#pragma once

#include "Math/Vec3.h"
#include "Math/Mat4.h"

class Camera
{
public:
    Camera(Vec3 position, Vec3 target, Vec3 up, float fovYRadians, float aspect, float nearPlane, float farPlane);

    Mat4 GetViewMatrix() const;
    Mat4 GetProjectionMatrix() const;
    Vec3 GetPosition() const { return m_position; }

private:
    Vec3 m_position;
    Vec3 m_target;
    Vec3 m_up;
    float m_fovY;
    float m_aspect;
    float m_near;
    float m_far;
};