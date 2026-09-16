cmake_minimum_required(VERSION 3.15)
project(SoftwareRasterizer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# All includes are written relative to src/ (e.g. "Core/Mesh.h", "Math/Vec3.h"),
# so this is the one place that path convention is anchored.
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

# -Wall/-Wextra are GCC/Clang syntax; MSVC's cl.exe doesn't understand them
# and fails to build (error D8021). /W4 is the closest MSVC equivalent.
if(MSVC)
    set(PROJECT_WARNING_FLAGS /W4)
else()
    set(PROJECT_WARNING_FLAGS -Wall -Wextra)
endif()

# --- Core engine library: math, mesh, framebuffer, camera, rasterizer. ---
add_library(rasterizer_lib STATIC
    ${SRC_ROOT}/Core/Mesh.cpp
    ${SRC_ROOT}/Core/Framebuffer.cpp
    ${SRC_ROOT}/Core/Camera.cpp
    ${SRC_ROOT}/Rendering/Rasterizer.cpp
    ${SRC_ROOT}/Rendering/Renderer.cpp
    ${SRC_ROOT}/Rendering/Clipping.cpp
)
target_include_directories(rasterizer_lib PUBLIC ${SRC_ROOT})
target_compile_options(rasterizer_lib PRIVATE ${PROJECT_WARNING_FLAGS})

# --- Main executable: renders one frame and saves it to cube_render.ppm. ---
add_executable(rasterizer_demo ${SRC_ROOT}/main.cpp)
target_link_libraries(rasterizer_demo PRIVATE rasterizer_lib)
target_compile_options(rasterizer_demo PRIVATE ${PROJECT_WARNING_FLAGS})
