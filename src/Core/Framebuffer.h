#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <limits>
#include <string>

struct Color
{
    uint8_t r, g, b;

    Color() : r(0), g(0), b(0) {}
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

    // Trivial and tiny - stays defined in the header (implicitly inline) like Vec3/Vec4.
    static Color FromIntensity(float intensity, Color base = Color(255, 255, 255))
    {
        intensity = std::clamp(intensity, 0.0f, 1.0f);
        return Color(
            static_cast<uint8_t>(base.r * intensity),
            static_cast<uint8_t>(base.g * intensity),
            static_cast<uint8_t>(base.b * intensity)
        );
    }
};

// Owns the color buffer and depth (z) buffer for one frame.
// Depth convention: NDC z in [-1, 1], SMALLER = closer to the camera.
// Cleared to +infinity so the first write to any pixel always passes.
class Framebuffer
{
public:
    Framebuffer(int width, int height);

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    // Called once per frame (or a handful of times) - fine to live in the .cpp.
    void Clear(Color clearColor);
    void SaveToPPM(const std::string& path) const;

    // --- Hot path: called once per candidate pixel, per triangle, every frame. ---
    // Kept 'inline' and defined here on purpose: across translation units this is
    // the difference between a real function call per pixel and the compiler
    // folding it straight into the rasterizer's inner loop.

    inline bool InBounds(int x, int y) const
    {
        return x >= 0 && x < m_width && y >= 0 && y < m_height;
    }

    inline bool TryWritePixel(int x, int y, float depth, Color color)
    {
        if (!InBounds(x, y))
            return false;

        size_t index = static_cast<size_t>(y) * m_width + x;
        if (depth >= m_depthBuffer[index])
            return false;

        m_depthBuffer[index] = depth;
        m_colorBuffer[index] = color;
        return true;
    }

    inline Color GetPixel(int x, int y) const
    {
        return m_colorBuffer[static_cast<size_t>(y) * m_width + x];
    }

    inline float GetDepth(int x, int y) const
    {
        return m_depthBuffer[static_cast<size_t>(y) * m_width + x];
    }

    // Fills 'dest' with RGBA8 pixel data (4 bytes/pixel, alpha always 255).
    // 'dest' must already be sized width*height*4 - this is the shape
    // sf::Texture::update() (and most GPU texture APIs) expect. Called once
    // per frame, not per candidate pixel, so it's fine to implement in the .cpp.
    void WriteRGBA8(std::uint8_t* dest) const;

    // Raw access to the color buffer - meant for bulk upload to a display
    // backend (e.g. copying into an sf::Texture once per frame), not for
    // per-pixel access from rasterizer code (use GetPixel/TryWritePixel there).
    inline const Color* GetColorData() const
    {
        return m_colorBuffer.data();
    }

private:
    int m_width, m_height;
    std::vector<Color> m_colorBuffer;
    std::vector<float> m_depthBuffer;
};
