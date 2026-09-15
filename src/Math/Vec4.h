#pragma once

#include "Math/Vec3.h"

// 4D vector with homogeneous coordinate 'w'.
// Used to carry vertices through the Model -> View -> Projection chain,
// since Mat4 * Vec4 is what produces clip space before the perspective divide.
struct Vec4
{
    float x, y, z, w;

    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    // w = 1 for points (affected by translation), w = 0 for directions (not affected).
    explicit Vec4(const Vec3& v, float w = 1.0f) : x(v.x), y(v.y), z(v.z), w(w) {}

    Vec3 ToVec3() const { return Vec3(x, y, z); }

    Vec4 operator+(const Vec4& other) const { return Vec4(x + other.x, y + other.y, z + other.z, w + other.w); }
    Vec4 operator-(const Vec4& other) const { return Vec4(x - other.x, y - other.y, z - other.z, w - other.w); }
    Vec4 operator*(float scalar) const { return Vec4(x * scalar, y * scalar, z * scalar, w * scalar); }

    // Divides x, y, z by w. This is the "perspective divide" step: clip space -> NDC.
    Vec3 PerspectiveDivide() const
    {
        if (std::fabs(w) < 1e-8f)
            return Vec3(x, y, z);
        float invW = 1.0f / w;
        return Vec3(x * invW, y * invW, z * invW);
    }
};