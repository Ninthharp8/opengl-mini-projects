#ifndef BUFFER_H
#define BUFFER_H

#include <glad/glad.h>

void createBuffers(GLuint &VAO, GLuint &VBO, GLuint &EBO, const GLfloat* vertices, const GLuint* indices, GLsizeiptr verticesSize, GLsizeiptr indicesSize);

#endif

