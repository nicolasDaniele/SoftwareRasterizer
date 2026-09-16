#pragma once

#include "Math/Vec4.h"

// Clips a single triangle - already transformed by MVP, but NOT YET divided
// by w - against the camera's near plane.
//
// Why this has to happen here and not in the Rasterizer: clip space is still
// an affine (linear) space, so interpolating between two vertices to find
// where an edge crosses the near plane is mathematically valid. Once you
// divide by w to get NDC, that linearity is gone - clipping after the divide
// would mean the damage (a vertex behind the camera producing a negative or
// near-zero w) has already happened.
//
// Returns how many triangles came out (0, 1, or 2) and writes them into
// outTriangles. A triangle entirely in front of the plane comes back
// unchanged as 1 triangle; entirely behind returns 0; a triangle straddling
// the plane is split into a quad and fan-triangulated into 2.
int ClipTriangleNearPlane(const Vec4& v0, const Vec4& v1, const Vec4& v2, float nearPlane, Vec4 outTriangles[2][3]);
