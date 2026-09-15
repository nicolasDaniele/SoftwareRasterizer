#include "Core/Mesh.h"
#include "Core/Framebuffer.h"
#include "Core/Camera.h"
#include "Rendering/Renderer.h"

int main()
{
    const int width = 800;
    const int height = 600;

    Framebuffer fb(width, height);
    fb.Clear(Color(20, 20, 30));

    Mesh cube = MeshFactory::CreateCube(2.0f);
    Mat4 model = Mat4::RotateY(0.6f) * Mat4::RotateX(0.3f);

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

    Renderer::RenderMesh(fb, cube, model, camera, lightDirection, /*backfaceCulling*/ true);

    fb.SaveToPPM("cube_render.ppm");
    return 0;
}
