# OpenGL Mini Projects 

Este repositorio contiene una colección de prácticas y proyectos desarrollados con OpenGL usando C++, organizados desde los fundamentos hasta temas avanzados como iluminación, carga de modelos, colisiones y normal mapping.

![Demostración de la app](assets/demo.gif)

<img src="assets/demo.gif" width="500"/>

Cada carpeta está numerada y contiene un proyecto autocontenible con su código fuente y recursos necesarios para compilar y ejecutar.

---

##  Índice de proyectos

| Nº  | Carpeta                               | Descripción                                          |
|-----|----------------------------------------|------------------------------------------------------------|
| 00  | `trifuerza-basico`                    | Primer renderizado de una trifuerza con triángulos básicos. |
| 01  | `triangulo-negro-basico`              | Triángulo estático negro sobre fondo blanco.              |
| 02  | `triangulo-rojo-escalado`             | Triángulo rojo con escala aplicada por `uniform`.         |
| 03  | `trifuerza-respirando-estatica`       | Trifuerza con animación de "respiración" (escala).        |
| 04  | `triangulo-colores-respirando`        | Triángulo con tres colores y animación.                   |
| 05  | `triangulo-respirando-textura-color`  | Triángulo con textura + colores + respiración.            |
| 06  | `triangulo-textura-respirando`        | Triángulo con animación de escala y textura.              |
| 07  | `trifuerza-respirando-rotacion`       | Trifuerza que respira y rota sobre su centro.             |
| 08  | `cruz`                                 | Renderizado de figura en forma de cruz.                   |
| 09  | `triangulo-colores-girando`           | Triángulo girando con interpolación de colores.           |
| 10  | `tetraedro-giratorio`                 | Rotación de un tetraedro 3D.                              |
| 11  | `carga-modelo-obj`                    | Carga de un modelo `.obj` desde archivo.                  |
| 11b | `carga-dos-modelos-obj`               | Carga y visualización de dos modelos `.obj`.              |
| 12  | `piramide-luz-rotatoria`              | Pirámide con iluminación dinámica y rotación.             |
| 13  | `normal-mapping`                      | Simulación de relieve mediante normal mapping.            |
| 14  | `senos-cosenos`                       | Animación con funciones seno y coseno.                    |
| 15  | `malla-senos-cosenos`                 | Malla deformada dinámicamente con funciones trigonométricas. |
| 16  | `colision-dos-obj`                    | Detección de colisiones entre dos objetos 2D.             |
| 17  | `colisiones-dos-obj-3d`               | Detección de colisiones entre objetos 3D.                 |
| 17b | `instancias`                          | Renderizado de múltiples instancias de un objeto.         |
| 18  | `room-lightmap-shader`                | Simulación de iluminación con lightmaps y shaders.        |
| 19  | `examen-instancias`                   | Proyecto tipo examen: renderizado con instanciación.      |
| 20  | `examen-carga-eje-coordenadas-3d-texturizado` | Proyecto de carga y texturizado de modelos con coordenadas 3D. |

---

##  Cómo compilar

Cada carpeta contiene archivos `.cpp` y recursos necesarios.Cada proyecto cuenta con su makefile:

```bash
all: 
	g++ -g --std=c++17 -I../include -L../lib ../src/*.cpp ../src/glad.c -lglfw3dll -o main

```

librerias :
- GLFW
- GLAD
- stb_image 
- libglfw3dll
- glm
- linmath
- KHR
---

## 📄 Nota

Este repositorio se realizó con fines educativos para reforzar conocimientos en gráficos computacionales usando OpenGL moderno (core profile).
