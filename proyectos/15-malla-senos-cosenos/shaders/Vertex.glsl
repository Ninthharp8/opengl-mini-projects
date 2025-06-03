#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;

// Salida hacia el shader de fragmentos
out vec3 vertexColor;

uniform mat4 MVP;

void main() {
    // Transformar el vértice con la matriz MVP
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

    // Asignar un color en función de la posición (puedes cambiar esta lógica)
    // vertexColor = vec3(abs(vertexPosition_modelspace.x), abs(vertexPosition_modelspace.y), 0.5);
    vertexColor = vec3(1.0, 0.0, 0.0);
}
