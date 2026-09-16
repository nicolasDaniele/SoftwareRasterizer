#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include "Core/Framebuffer.h"

// Thin wrapper around an SFML window, used strictly as a pixel-buffer
// display: SFML never touches the 3D pipeline. Every frame it receives the
// already-fully-rendered Framebuffer and blits it to the screen as a single
// textured quad - this is "Opcion B" from the test statement (SDL/SFML/Raylib
// used only as a screen buffer, no 3D rendering functions of the library).
class Window
{
public:
    Window(unsigned int width, unsigned int height, const std::string& title);

    bool IsOpen() const;
    void PollEvents();
    void Present(const Framebuffer& fb);

    static bool IsKeyPressed(sf::Keyboard::Key key);

private:
    sf::RenderWindow m_window;
    sf::Texture m_texture;
    sf::Sprite m_sprite;
    std::vector<std::uint8_t> m_pixelBuffer; // RGBA8 scratch buffer, refilled every frame
};
