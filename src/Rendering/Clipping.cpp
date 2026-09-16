#include "Rendering/Clipping.h"

namespace
{
    // Signed distance from the near plane: positive = in front (keep),
    // negative = behind (cut). With our Perspective matrix, w_clip = -z_view,
    // and the near plane sits at z_view = -nearPlane, so "at the plane" is
    // exactly w_clip == nearPlane - no epsilon guessing needed, it's tied
    // directly to the same 'near' value the camera was built with.
    inline float NearPlaneDistance(const Vec4& v, float nearPlane)
    {
        return v.w - nearPlane;
    }

    // Sutherland-Hodgman clip of a convex polygon (here always a triangle,
    // so inCount == 3) against a single plane. Walks each edge and keeps the
    // portion on the "inside" side, inserting a new vertex wherever an edge
    // crosses the plane. Output has at most inCount + 1 vertices (4 for a
    // triangle input).
    int ClipPolygonAgainstNearPlane(const Vec4* inVerts, int inCount, float nearPlane, Vec4* outVerts)
    {
        int outCount = 0;

        for (int i = 0; i < inCount; ++i)
        {
            const Vec4& current = inVerts[i];
            const Vec4& next = inVerts[(i + 1) % inCount];

            float currentDist = NearPlaneDistance(current, nearPlane);
            float nextDist = NearPlaneDistance(next, nearPlane);

            bool currentInside = currentDist >= 0.0f;
            bool nextInside = nextDist >= 0.0f;

            if (currentInside)
                outVerts[outCount++] = current;

            if (currentInside != nextInside)
            {
                // Edge crosses the plane - linear interpolation is valid here
                // because we're still in (affine) clip space, pre-divide.
                float t = currentDist / (currentDist - nextDist);
                outVerts[outCount++] = current + (next - current) * t;
            }
        }

        return outCount;
    }
}

int ClipTriangleNearPlane(const Vec4& v0, const Vec4& v1, const Vec4& v2, float nearPlane, Vec4 outTriangles[2][3])
{
    Vec4 inVerts[3] = { v0, v1, v2 };
    Vec4 clipped[4];
    int clippedCount = ClipPolygonAgainstNearPlane(inVerts, 3, nearPlane, clipped);

    if (clippedCount < 3)
        return 0; // fully behind the near plane (or degenerate) - nothing to draw

    // Fan-triangulate the resulting convex polygon from its first vertex:
    // 3 vertices -> 1 triangle (the original, untouched), 4 -> 2 triangles.
    int triangleCount = clippedCount - 2;
    for (int i = 0; i < triangleCount; ++i)
    {
        outTriangles[i][0] = clipped[0];
        outTriangles[i][1] = clipped[i + 1];
        outTriangles[i][2] = clipped[i + 2];
    }

    return triangleCount;
}
