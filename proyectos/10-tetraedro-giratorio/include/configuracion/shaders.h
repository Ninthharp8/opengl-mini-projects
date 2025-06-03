#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>

GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
void checkCompileErrors(GLuint shader, const std::string& type);
std::string readShaderSource(const std::string& filePath);

#endif
