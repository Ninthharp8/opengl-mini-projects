#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;

void main() {
    gl_Position = vec4(vertexPosition_modelspace, 1.0);
    gl_PointSize = 5.0; // Tamaño del punto si usas GL_POINTS
}
