#include "Rendering/Rasterizer.h"
#include <algorithm>
#include <cmath>

namespace
{
    // Signed area * 2 of triangle (a, b, c). This doubles as the barycentric
    // "edge function": positive on one side of the directed edge a->b, 
    // negative on the other, zero exactly on the line.
    inline float EdgeFunction(const ScreenVertex& a, const ScreenVertex& b, const ScreenVertex& p)
    {
        return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
    }
}

void Rasterizer::DrawTriangle(Framebuffer& fb, const ScreenVertex& v0, const ScreenVertex& v1, const ScreenVertex& v2, Color color)
{
    float area = EdgeFunction(v0, v1, v2);

    // Degenerate triangle (collapsed to a line or point) - nothing to draw.
    if (std::fabs(area) < 1e-8f)
        return;

    // Bounding box in screen space, clamped to the framebuffer bounds.
    int minX = std::max(0, static_cast<int>(std::floor(std::min({ v0.x, v1.x, v2.x }))));
    int maxX = std::min(fb.GetWidth() - 1, static_cast<int>(std::ceil(std::max({ v0.x, v1.x, v2.x }))));
    int minY = std::max(0, static_cast<int>(std::floor(std::min({ v0.y, v1.y, v2.y }))));
    int maxY = std::min(fb.GetHeight() - 1, static_cast<int>(std::ceil(std::max({ v0.y, v1.y, v2.y }))));

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            ScreenVertex p{ x + 0.5f, y + 0.5f, 0.0f }; // sample at pixel center

            float w0 = EdgeFunction(v1, v2, p);
            float w1 = EdgeFunction(v2, v0, p);
            float w2 = EdgeFunction(v0, v1, p);

            // Inside test: the pixel must fall on the same side of all three
            // edges as the triangle's own signed area - this works regardless
            // of whether the triangle happens to be winded CW or CCW in screen space.
            bool inside = (area > 0.0f)
                ? (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f)
                : (w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f);

            if (!inside)
                continue;

            // Normalize edge functions into barycentric weights (they sum to 1)
            // and use them to interpolate depth across the triangle.
            float b0 = w0 / area;
            float b1 = w1 / area;
            float b2 = w2 / area;
            float depth = b0 * v0.depth + b1 * v1.depth + b2 * v2.depth;

            fb.TryWritePixel(x, y, depth, color);
        }
    }
}
