#version 330 core

out vec4 color;
uniform vec3 inputColor;

void main(){
    color = vec4(inputColor, 1.0);
}
