# 3.21 for $<TARGET_RUNTIME_DLLS:...> below (auto-copying SFML's DLLs on Windows).
cmake_minimum_required(VERSION 3.21)
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

# Vendored, precompiled SFML 3.1 (External/SFML3/{include,lib,bin}) - point
# find_package() at it instead of relying on a system install. The official
# prebuilt Windows packages ship their own lib/cmake/SFML/*.cmake config
# files inside the lib/ folder you copied, which is what makes this work.
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/External/SFML3)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

# --- Core engine library: math, mesh, framebuffer, camera, rasterizer.
#     Does not depend on SFML at all - SFML only enters at the main.cpp
#     level, as a display layer for the pixel buffer this already produces. ---
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

# --- Main executable ---
add_executable(rasterizer_demo
    ${SRC_ROOT}/main.cpp
    ${SRC_ROOT}/App/Window.cpp
)
target_link_libraries(rasterizer_demo PRIVATE rasterizer_lib SFML::Graphics SFML::Window SFML::System)
target_compile_options(rasterizer_demo PRIVATE ${PROJECT_WARNING_FLAGS})

# On Windows, SFML's DLLs (sfml-graphics-3.dll, etc.) need to sit next to the
# .exe to run at all. This copies whatever shared-library dependencies the
# target actually has right after each build, so you don't have to do it
# by hand every time you rebuild.
if(WIN32)
    add_custom_command(TARGET rasterizer_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:rasterizer_demo>
            $<TARGET_FILE_DIR:rasterizer_demo>
        COMMAND_EXPAND_LISTS
    )
endif()
