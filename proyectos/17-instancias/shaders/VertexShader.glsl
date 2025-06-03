#version 330 core

layout(location = 0) in vec2 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 instanceOffset;  // Offset for instancing

out vec3 fragmentColor;

uniform mat4 MVP;
uniform mat4 Transformation;

void main() {
    // Apply the transformation matrix to each instance
    vec4 transformedPosition = Transformation * vec4(vertexPosition_modelspace + instanceOffset,0.0, 1.0);
    
    // Apply the MVP matrix to project the transformed position
    gl_Position = MVP * transformedPosition;

    // Pass the color to the fragment shader
    fragmentColor = vertexColor;
}
