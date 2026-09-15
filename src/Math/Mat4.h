#pragma once

#include <cmath>
#include "Math/Vec3.h"
#include "Math/Vec4.h"

// 4x4 matrix stored column-major: m[col * 4 + row]
// This means a translation lives in m[12], m[13], m[14].
struct Mat4
{
    float m[16];

    // Sets identity directly (does NOT call Identity(), to avoid the default
    // constructor recursing into itself through Mat4's own default construction).
    Mat4()
    {
        for (int i = 0; i < 16; ++i) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    // Access element at (row, col).
    float& At(int row, int col) { return m[col * 4 + row]; }
    float At(int row, int col) const { return m[col * 4 + row]; }

    static Mat4 Identity() { return Mat4(); }

    // Column-major matrix multiplication: result = a * b.
    // Applying (a * b) to a vector means "apply b first, then a" -
    // this is the order you rely on to build Model = T * R * S.
    static Mat4 Multiply(const Mat4& a, const Mat4& b)
    {
        Mat4 result{};
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k)
                    sum += a.At(row, k) * b.At(k, col);
                result.At(row, col) = sum;
            }
        }
        return result;
    }

    Mat4 operator*(const Mat4& other) const { return Multiply(*this, other); }

    // Transforms a homogeneous vector: result = M * v.
    Vec4 operator*(const Vec4& v) const
    {
        return Vec4(
            At(0, 0) * v.x + At(0, 1) * v.y + At(0, 2) * v.z + At(0, 3) * v.w,
            At(1, 0) * v.x + At(1, 1) * v.y + At(1, 2) * v.z + At(1, 3) * v.w,
            At(2, 0) * v.x + At(2, 1) * v.y + At(2, 2) * v.z + At(2, 3) * v.w,
            At(3, 0) * v.x + At(3, 1) * v.y + At(3, 2) * v.z + At(3, 3) * v.w
        );
    }

    static Mat4 Translate(const Vec3& t)
    {
        Mat4 result = Identity();
        result.At(0, 3) = t.x;
        result.At(1, 3) = t.y;
        result.At(2, 3) = t.z;
        return result;
    }

    static Mat4 Scale(const Vec3& s)
    {
        Mat4 result = Identity();
        result.At(0, 0) = s.x;
        result.At(1, 1) = s.y;
        result.At(2, 2) = s.z;
        return result;
    }

    static Mat4 RotateX(float radians)
    {
        Mat4 result = Identity();
        float c = std::cos(radians), s = std::sin(radians);
        result.At(1, 1) = c;  result.At(1, 2) = -s;
        result.At(2, 1) = s;  result.At(2, 2) = c;
        return result;
    }

    static Mat4 RotateY(float radians)
    {
        Mat4 result = Identity();
        float c = std::cos(radians), s = std::sin(radians);
        result.At(0, 0) = c;   result.At(0, 2) = s;
        result.At(2, 0) = -s;  result.At(2, 2) = c;
        return result;
    }

    static Mat4 RotateZ(float radians)
    {
        Mat4 result = Identity();
        float c = std::cos(radians), s = std::sin(radians);
        result.At(0, 0) = c;  result.At(0, 1) = -s;
        result.At(1, 0) = s;  result.At(1, 1) = c;
        return result;
    }

    // Rotation around an arbitrary normalized axis (Rodrigues' rotation formula).
    static Mat4 RotateAxisAngle(const Vec3& axis, float radians)
    {
        Vec3 a = axis.Normalized();
        float c = std::cos(radians), s = std::sin(radians), t = 1.0f - c;

        Mat4 result = Identity();
        result.At(0, 0) = t * a.x * a.x + c;
        result.At(0, 1) = t * a.x * a.y - s * a.z;
        result.At(0, 2) = t * a.x * a.z + s * a.y;

        result.At(1, 0) = t * a.x * a.y + s * a.z;
        result.At(1, 1) = t * a.y * a.y + c;
        result.At(1, 2) = t * a.y * a.z - s * a.x;

        result.At(2, 0) = t * a.x * a.z - s * a.y;
        result.At(2, 1) = t * a.y * a.z + s * a.x;
        result.At(2, 2) = t * a.z * a.z + c;

        return result;
    }

    // Builds the View matrix: transforms world space into camera space,
    // where the camera sits at the origin looking down -Z.
    static Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up)
    {
        Vec3 forward = (target - eye).Normalized();          // camera looks towards -forward
        Vec3 right = Vec3::Cross(forward, up).Normalized();
        Vec3 camUp = Vec3::Cross(right, forward);

        Mat4 result = Identity();

        // View transforms world space into camera space.
        // The camera basis describes camera -> world, so the rotation used here
        // is its inverse. Because the basis is orthonormal, inverse = transpose.
        result.At(0, 0) = right.x;    result.At(0, 1) = right.y;    result.At(0, 2) = right.z;
        result.At(1, 0) = camUp.x;    result.At(1, 1) = camUp.y;    result.At(1, 2) = camUp.z;
        result.At(2, 0) = -forward.x; result.At(2, 1) = -forward.y; result.At(2, 2) = -forward.z;

        // The translation moves the camera position (eye) to the origin and
        // expresses that translation in camera coordinates.
        result.At(0, 3) = -Vec3::Dot(right, eye);
        result.At(1, 3) = -Vec3::Dot(camUp, eye);
        result.At(2, 3) = Vec3::Dot(forward, eye);

        return result;
    }

    // Builds the Projection matrix: perspective frustum -> clip space.
    // fovYRadians is the *vertical* field of view.
    static Mat4 Perspective(float fovYRadians, float aspect, float nearPlane, float farPlane)
    {
        Mat4 result{};
        for (int i = 0; i < 16; ++i) result.m[i] = 0.0f;

        float tanHalfFov = std::tan(fovYRadians * 0.5f);

        result.At(0, 0) = 1.0f / (aspect * tanHalfFov);
        result.At(1, 1) = 1.0f / tanHalfFov;
        result.At(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result.At(2, 3) = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        result.At(3, 2) = -1.0f;  // this is what carries -viewSpaceZ into w, enabling the perspective divide

        return result;
    }

    Mat4 Transposed() const
    {
        Mat4 result{};
        for (int row = 0; row < 4; ++row)
            for (int col = 0; col < 4; ++col)
                result.At(col, row) = At(row, col);
        return result;
    }

    // Transforms a normal correctly under non-uniform scale: normals need the
    // inverse-transpose of the model matrix, not the model matrix itself.
    // For rotation-only or uniform-scale models, Transposed() of the upper 3x3
    // would suffice, but this general form is here so it's correct either way.
    static Vec3 TransformNormal(const Mat4& normalMatrix, const Vec3& normal)
    {
        Vec4 n = normalMatrix * Vec4(normal, 0.0f);
        return n.ToVec3();
    }
};