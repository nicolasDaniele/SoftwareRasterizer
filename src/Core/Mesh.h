#pragma once

#include <vector>
#include <cstdint>
#include "Math/Vec3.h"

// A vertex only needs a position: since shading will be flat (per-triangle),
// the normal will be derived from the triangle's own vertices, not stored per-vertex.
struct Vertex
{
    Vec3 position;
};

struct Triangle
{
    uint32_t i0, i1, i2;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;

    Vec3 ComputeFaceNormal(const Triangle& tri) const;
};

class MeshFactory
{
public:
    static Mesh CreateCube(float size);
    static Mesh CreatePyramid(float baseSize, float height);
};