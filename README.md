# Software Rasterizer

Rasterizador 3D por software escrito en C++17. Toma una malla simple (cubo),
la transforma con matrices propias (Model → View → Projection), la proyecta
a espacio de pantalla y la dibuja píxel a píxel con un rasterizador de
triángulos por coordenadas baricéntricas, z-buffer y *flat shading*. El
resultado se guarda como imagen (`cube_render.ppm`).

No se usa el pipeline gráfico de ninguna API (OpenGL/DirectX/Vulkan) ni
funciones de renderizado 3D de terceros: toda la matemática de vectores,
matrices, proyección y rasterizado está implementada desde cero en
`src/Math` y `src/Rendering`. Esta rama no tiene ninguna dependencia
externa — es la Opción A del enunciado (salida directa a archivo de imagen).

> Existe otra rama con una ventana SFML que muestra el mismo render en
> vivo y permite rotar y trasladar el cubo con el teclado (Opción B),
> ver la rama sfml_window_display.

## Estructura del proyecto

```
SoftwareRasterizer/
├── CMakeLists.txt
└── src/
    ├── main.cpp
    ├── Math/          # Header-only: Vec3, Vec4, Mat4
    ├── Core/          # Mesh, Framebuffer, Camera
    └── Rendering/      # Rasterizer, Renderer (arma el pipeline completo)
```

## Compilar

Sin dependencias externas — solo CMake y un compilador con soporte C++17.

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Ejecutar

```bash
./rasterizer_demo
```

Genera `cube_render.ppm` en el directorio desde el que se ejecuta. El
formato PPM (P6) lo abre cualquier visor de imágenes moderno; en caso de 
necesitar convertirlo a PNG/JPG, cualquier conversor (ImageMagick, GIMP, Convertio, etc.) 
debería ser capaz de convertirlo.

## Decisiones de diseño

- **Convención de profundidad:** NDC con `z` en `[-1, 1]`, donde *menor
  valor = más cerca* de la cámara. El framebuffer se limpia en `+infinito`
  para que el primer píxel escrito en cualquier posición siempre pase el
  test de profundidad.
- **Espacio de pantalla:** NDC es Y-arriba (como el resto de la matemática
  del motor); pantalla es Y-abajo (fila 0 = arriba de la imagen). Ese flip
  ocurre en un único lugar (`NdcToScreen`, en `Rendering/Rasterizer.h`), así
  que el resto del pipeline no necesita saber que existe.
- **Backface culling en espacio de mundo**, no en pantalla: se compara la
  normal de la cara contra la dirección hacia la cámara *antes* de
  proyectar. Evita depender del signo del área 2D del triángulo proyectado,
  que cambia según la convención de winding que se use.
- **Dirección de la luz:** `lightDirection` es la dirección en la que
  *viaja* la luz (no hacia dónde apunta la fuente), así que el shading usa
  `dot(normal, -lightDir)`.
- **Normales por cara, no por vértice:** como el shading es plano, la
  normal se calcula geométricamente a partir de los tres vértices del
  triángulo (`Mesh::ComputeFaceNormal`), lo que permite compartir vértices
  entre caras sin necesidad de duplicarlos.
- **Separación header-only vs .cpp:** `Math/` (Vec3, Vec4, Mat4) quedó
  header-only a propósito, porque se llama constantemente desde el hot path
  (una transformación por vértice, productos escalares por píxel candidato)
  y perder el inlining entre unidades de traducción ahí se nota. El resto
  (`Mesh`, `Framebuffer`, `Camera`) se llama con mucha menor frecuencia por
  frame, así que vive en `.cpp` sin costo real — salvo los métodos de
  `Framebuffer` que sí se llaman una vez por píxel candidato
  (`TryWritePixel`, `InBounds`, `GetPixel`), que se dejaron `inline` en el
  header por la misma razón que `Math/`.

## Posibles extensiones

- Pirámide como segunda malla de prueba (`MeshFactory::CreatePyramid` ya
  está implementado).
- Órbita de cámara alrededor del mesh.
- Animación/interpolación entre dos posturas.