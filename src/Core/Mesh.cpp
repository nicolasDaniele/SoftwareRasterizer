#include "Core/Mesh.h"

Vec3 Mesh::ComputeFaceNormal(const Triangle& tri) const
{
    const Vec3& v0 = vertices[tri.i0].position;
    const Vec3& v1 = vertices[tri.i1].position;
    const Vec3& v2 = vertices[tri.i2].position;

    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    return Vec3::Cross(edge1, edge2).Normalized();
}

Mesh MeshFactory::CreateCube(float size)
{
    float h = size * 0.5f;

    Mesh mesh;
    mesh.vertices = {
        { Vec3(-h, -h, -h) }, // 0
        { Vec3( h, -h, -h) }, // 1
        { Vec3( h,  h, -h) }, // 2
        { Vec3(-h,  h, -h) }, // 3
        { Vec3(-h, -h,  h) }, // 4
        { Vec3( h, -h,  h) }, // 5
        { Vec3( h,  h,  h) }, // 6
        { Vec3(-h,  h,  h) }, // 7
    };

    mesh.triangles = {
        // back  (z = -h, normal -Z)
        {0, 2, 1}, {0, 3, 2},
        // front (z = +h, normal +Z)
        {4, 5, 6}, {4, 6, 7},
        // left  (x = -h, normal -X)
        {0, 4, 7}, {0, 7, 3},
        // right (x = +h, normal +X)
        {1, 2, 6}, {1, 6, 5},
        // bottom (y = -h, normal -Y)
        {0, 1, 5}, {0, 5, 4},
        // top    (y = +h, normal +Y)
        {3, 7, 6}, {3, 6, 2},
    };

    return mesh;
}

Mesh MeshFactory::CreatePyramid(float baseSize, float height)
{
    float h = baseSize * 0.5f;

    Mesh mesh;
    mesh.vertices = {
        { Vec3(-h, 0.0f, -h) },        // 0 base
        { Vec3( h, 0.0f, -h) },        // 1 base
        { Vec3( h, 0.0f,  h) },        // 2 base
        { Vec3(-h, 0.0f,  h) },        // 3 base
        { Vec3(0.0f, height, 0.0f) },  // 4 apex
    };

    mesh.triangles = {
        // base (y = 0, normal -Y, viewed from below)
        {0, 1, 2}, {0, 2, 3},
        // sides
        {0, 4, 1},
        {1, 4, 2},
        {2, 4, 3},
        {3, 4, 0},
    };

    return mesh;
}