#version 330 core

in vec3 fragmentColor;
out vec3 color;

void main() {
    // Output the interpolated color
    color = fragmentColor;
}
