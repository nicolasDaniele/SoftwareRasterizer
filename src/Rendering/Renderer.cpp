#include "Rendering/Renderer.h"
#include "Rendering/Rasterizer.h"
#include <algorithm>

void Renderer::RenderMesh(
    Framebuffer& fb,
    const Mesh& mesh,
    const Mat4& modelMatrix,
    const Camera& camera,
    const Vec3& lightDirection,
    bool backfaceCulling)
{
    Mat4 view = camera.GetViewMatrix();
    Mat4 projection = camera.GetProjectionMatrix();
    Mat4 mvp = projection * view * modelMatrix;

    Vec3 lightDirNormalized = lightDirection.Normalized();

    for (const Triangle& tri : mesh.triangles)
    {
        // Face normal transformed into world space - flat shading needs the
        // normal AFTER the model's own rotation, not the local-space one.
        Vec3 localNormal = mesh.ComputeFaceNormal(tri);
        Vec3 worldNormal = Mat4::TransformNormal(modelMatrix, localNormal).Normalized();

        // Backface culling done in WORLD space, before projecting: compare the
        // face normal against the direction from the face to the camera. This
        // sidesteps the 2D screen-space winding question entirely - it doesn't
        // care whether the projected triangle ends up CW or CCW on screen.
        Vec3 worldV0 = (modelMatrix * Vec4(mesh.vertices[tri.i0].position, 1.0f)).ToVec3();
        Vec3 toCamera = (camera.GetPosition() - worldV0).Normalized();
        if (backfaceCulling && Vec3::Dot(worldNormal, toCamera) <= 0.0f)
            continue;

        // Transform the three vertices: local -> clip space -> perspective divide -> screen.
        uint32_t indices[3] = { tri.i0, tri.i1, tri.i2 };
        ScreenVertex screenVerts[3];
        for (int i = 0; i < 3; ++i)
        {
            Vec4 clipSpace = mvp * Vec4(mesh.vertices[indices[i]].position, 1.0f);
            Vec3 ndc = clipSpace.PerspectiveDivide();
            screenVerts[i] = NdcToScreen(ndc, fb.GetWidth(), fb.GetHeight());
        }

        // Flat shading: how directly the face points away from the light.
        float intensity = std::max(0.0f, Vec3::Dot(worldNormal, -lightDirNormalized));
        Color color = Color::FromIntensity(intensity);

        Rasterizer::DrawTriangle(fb, screenVerts[0], screenVerts[1], screenVerts[2], color);
    }
}
