#include "App/Window.h"
#include <stdexcept>

namespace
{
    // sf::Sprite has no default constructor and captures its texture rect at
    // construction time, so the texture MUST already be the right size before
    // the Sprite is built. This helper lets m_texture's member-initializer
    // produce an already-resized Texture, so by the time m_sprite's own
    // initializer runs (right after, in declaration order), it sees the
    // correct size.
    sf::Texture CreateTexture(unsigned int width, unsigned int height)
    {
        sf::Texture texture;
        if (!texture.resize({ width, height }))
            throw std::runtime_error("Window: failed to create the display texture");
        return texture;
    }
}

Window::Window(unsigned int width, unsigned int height, const std::string& title)
    : m_window(sf::VideoMode({ width, height }), title)
    , m_texture(CreateTexture(width, height))
    , m_sprite(m_texture)
    , m_pixelBuffer(static_cast<size_t>(width) * height * 4)
{
    m_window.setFramerateLimit(60);
}

bool Window::IsOpen() const
{
    return m_window.isOpen();
}

void Window::PollEvents()
{
    while (const std::optional event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            m_window.close();
    }
}

void Window::Present(const Framebuffer& fb)
{
    fb.WriteRGBA8(m_pixelBuffer.data());
    m_texture.update(m_pixelBuffer.data());

    m_window.clear();
    m_window.draw(m_sprite);
    m_window.display();
}

bool Window::IsKeyPressed(sf::Keyboard::Key key)
{
    return sf::Keyboard::isKeyPressed(key);
}
