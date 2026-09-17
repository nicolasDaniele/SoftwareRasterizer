# Software Rasterizer

Rasterizador 3D por software escrito en C++17. Toma una malla simple (cubo),
la transforma con matrices propias (Model → View → Projection), la proyecta
a espacio de pantalla y la dibuja píxel a píxel con un rasterizador de
triángulos por coordenadas baricéntricas, z-buffer y *flat shading*.

No se usa el pipeline gráfico de ninguna API (OpenGL/DirectX/Vulkan) ni
funciones de renderizado 3D de terceros. Toda la matemática de vectores,
matrices, proyección y rasterizado está implementada desde cero en
`src/Math` y `src/Rendering`. SFML se usa **únicamente** como buffer de
pantalla: recibe el framebuffer ya renderizado y lo vuelca a una ventana,
sin tocar en ningún momento el pipeline 3D (Opción B del enunciado).

> Para la versión sin dependencias externas que vuelca el resultado a un
> archivo de imagen (Opción A), ver la rama `main`.

## Estructura del proyecto

```
SoftwareRasterizer/
├── CMakeLists.txt
├── External/
│   └── SFML3/              # SFML 3.1 precompilado (no incluido en el repo, ver más abajo)
└── src/
    ├── main.cpp
    ├── Math/                # Header-only: Vec3, Vec4, Mat4
    ├── Core/                # Mesh, Framebuffer, Camera
    ├── Rendering/           # Rasterizer, Renderer (arma el pipeline completo)
    └── App/                 # Window (única parte del proyecto que conoce SFML)
```

## Compilar

### Requisitos
- CMake 3.21+
- Compilador con soporte C++17
- SFML 3.1

### SFML

Este proyecto espera SFML 3.1 precompilado en `External/SFML3/`, con esta
forma (las carpetas `bin/`, `include/` y `lib/` del paquete oficial):

```
External/SFML3/
├── bin/       (DLLs, en Windows)
├── include/
└── lib/
```

Descargalo desde la [página de descargas de SFML](https://www.sfml-dev.org/download/sfml/3.1.0/)
eligiendo el paquete que coincida con tu compilador (MSVC o MinGW) y
arquitectura, y copiá esas tres carpetas ahí. No hace falta `doc/` ni
`examples/`.

### Build

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

El ejecutable queda en `build/` (o `build/Release/` con generadores multi-config
como Visual Studio). En Windows, el propio build copia las DLLs de SFML
necesarias junto al `.exe` automáticamente.

## Controles

| Tecla | Acción |
|---|---|
| ← / → | Rota el cubo sobre el eje Y |
| ↑ / ↓ | Traslada el cubo en el eje Y |
| Cerrar ventana | Termina el programa |

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
- Órbita de cámara alrededor del mesh como alternativa a rotar el objeto.
- Animación/interpolación entre dos posturas.
