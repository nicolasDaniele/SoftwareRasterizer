#pragma once

#include <cmath>

// 3D vector used for positions, directions and normals in local/world/view space.
struct Vec3
{
    float x, y, z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }

    Vec3& operator+=(const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vec3& operator-=(const Vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vec3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }

    static float Dot(const Vec3& a, const Vec3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    // Right-handed cross product: Cross(x, y) = z
    static Vec3 Cross(const Vec3& a, const Vec3& b)
    {
        return Vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    static Vec3 Lerp(const Vec3& a, const Vec3& b, float t)
    {
        return a + (b - a) * t;
    }

    float Length() const { return std::sqrt(x * x + y * y + z * z); }
    float LengthSquared() const { return x * x + y * y + z * z; }

    Vec3 Normalized() const
    {
        float len = Length();
        if (len < 1e-8f)
            return Vec3(0.0f, 0.0f, 0.0f);
        return Vec3(x / len, y / len, z / len);
    }
};

inline Vec3 operator*(float scalar, const Vec3& v) { return v * scalar; }