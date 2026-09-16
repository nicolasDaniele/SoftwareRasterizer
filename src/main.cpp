#include "Core/Mesh.h"
#include "Core/Framebuffer.h"
#include "Core/Camera.h"
#include "Rendering/Renderer.h"
#include "App/Window.h"

int main()
{
    const unsigned int width = 800;
    const unsigned int height = 600;

    Framebuffer fb(width, height);
    Window window(width, height, "Rasterizer 3000");

    Mesh cube = MeshFactory::CreateCube(2.0f);

    float fovY = 60.0f * 3.14159265f / 180.0f;
    Camera camera(
        Vec3(0.0f, 0.0f, 6.0f),  // eye
        Vec3(0.0f, 0.0f, 0.0f),  // target
        Vec3(0.0f, 1.0f, 0.0f),  // up
        fovY,
        static_cast<float>(width) / height,
        0.1f, 100.0f);

    // Light traveling down and slightly forward-left.
    Vec3 lightDirection(0.5f, -1.0f, -0.3f);

    float rotationAngle = 0.6f; // start at the same angle main.cpp used to demo, just for a nicer initial frame
    const float rotationSpeed = 0.03f; // radians/frame - simple and enough for this test; a real game loop would scale by delta time

    float verticalOffset = 0.0f;
    const float moveSpeed = 0.03f; // world units/frame

    while (window.IsOpen())
    {
        window.PollEvents();

        if (Window::IsKeyPressed(sf::Keyboard::Key::Left))
            rotationAngle -= rotationSpeed;
        if (Window::IsKeyPressed(sf::Keyboard::Key::Right))
            rotationAngle += rotationSpeed;

        if (Window::IsKeyPressed(sf::Keyboard::Key::Up))
            verticalOffset += moveSpeed;
        if (Window::IsKeyPressed(sf::Keyboard::Key::Down))
            verticalOffset -= moveSpeed;

        // Translate AFTER rotate: the cube spins in place around its own
        // center first, then that already-rotated result gets shifted up or
        // down in world space - if the order were reversed, moving up would
        // also drag the rotation's pivot point away from the cube's center.
        Mat4 model = Mat4::Translate(Vec3(0.0f, verticalOffset, 0.0f)) * Mat4::RotateY(rotationAngle);

        fb.Clear(Color(20, 20, 30));
        Renderer::RenderMesh(fb, cube, model, camera, lightDirection, /*backfaceCulling*/ true);

        window.Present(fb);
    }

    return 0;
}
