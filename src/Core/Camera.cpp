#include "Core/Camera.h"

Camera::Camera(Vec3 position, Vec3 target, Vec3 up, float fovYRadians, float aspect, float nearPlane, float farPlane)
    : m_position(position)
    , m_target(target)
    , m_up(up)
    , m_fovY(fovYRadians)
    , m_aspect(aspect)
    , m_near(nearPlane)
    , m_far(farPlane)
{
}

Mat4 Camera::GetViewMatrix() const
{
    return Mat4::LookAt(m_position, m_target, m_up);
}

Mat4 Camera::GetProjectionMatrix() const
{
    return Mat4::Perspective(m_fovY, m_aspect, m_near, m_far);
}