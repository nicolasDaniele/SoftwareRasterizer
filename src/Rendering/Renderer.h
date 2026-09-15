#pragma once

#include "Core/Mesh.h"
#include "Core/Camera.h"
#include "Core/Framebuffer.h"

class Renderer
{
public:
    // Renders 'mesh' with the given model transform into 'fb', lit by a single
    // directional light. lightDirection points the way the light TRAVELS
    // (surface-ward), matching how you'd typically author it ("the light is
    // coming from up and to the left" -> direction points down-right).
    static void RenderMesh(
        Framebuffer& fb,
        const Mesh& mesh,
        const Mat4& modelMatrix,
        const Camera& camera,
        const Vec3& lightDirection,
        bool backfaceCulling = true);
};
