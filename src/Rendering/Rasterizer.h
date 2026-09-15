#pragma once

#include "Core/Framebuffer.h"
#include "Math/Vec3.h"

// A vertex already fully transformed into screen space: x/y are pixel
// coordinates (not NDC), depth follows the Framebuffer's convention
// (NDC z in [-1, 1], smaller = closer).
struct ScreenVertex
{
    float x, y, depth;
};

// Maps a point in NDC space ([-1, 1] on all three axes) into screen space.
// NDC is Y-up (matching the math convention used everywhere else in the
// engine); screen space is Y-down (row 0 is the top of the image), so this
// is also where that flip happens - nowhere else needs to know about it.
inline ScreenVertex NdcToScreen(const Vec3& ndc, int screenWidth, int screenHeight)
{
    float x = (ndc.x * 0.5f + 0.5f) * screenWidth;
    float y = (1.0f - (ndc.y * 0.5f + 0.5f)) * screenHeight;
    return ScreenVertex{ x, y, ndc.z };
}

class Rasterizer
{
public:
    // Fills the triangle with a single flat color, depth-testing every
    // candidate pixel against the framebuffer's z-buffer.
    static void DrawTriangle(Framebuffer& fb, const ScreenVertex& v0, const ScreenVertex& v1, const ScreenVertex& v2, Color color);
};
