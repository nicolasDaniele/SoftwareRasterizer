#include "Core/Framebuffer.h"
#include <fstream>

Framebuffer::Framebuffer(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_colorBuffer(static_cast<size_t>(width) * height)
    , m_depthBuffer(static_cast<size_t>(width) * height)
{
    Clear(Color(0, 0, 0));
}

void Framebuffer::Clear(Color clearColor)
{
    std::fill(m_colorBuffer.begin(), m_colorBuffer.end(), clearColor);
    std::fill(m_depthBuffer.begin(), m_depthBuffer.end(), std::numeric_limits<float>::infinity());
}

// Binary PPM (P6) - simplest image format that needs zero external libraries.
void Framebuffer::SaveToPPM(const std::string& path) const
{
    std::ofstream file(path, std::ios::binary);
    file << "P6\n" << m_width << " " << m_height << "\n255\n";
    for (const Color& c : m_colorBuffer)
    {
        file.put(static_cast<char>(c.r));
        file.put(static_cast<char>(c.g));
        file.put(static_cast<char>(c.b));
    }
}

void Framebuffer::WriteRGBA8(std::uint8_t* dest) const
{
    for (size_t i = 0; i < m_colorBuffer.size(); ++i)
    {
        const Color& c = m_colorBuffer[i];
        dest[i * 4 + 0] = c.r;
        dest[i * 4 + 1] = c.g;
        dest[i * 4 + 2] = c.b;
        dest[i * 4 + 3] = 255;
    }
}
