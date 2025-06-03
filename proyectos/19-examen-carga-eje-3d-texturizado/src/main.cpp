// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <glad/glad.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <../include/common/shader.hpp>
#include <../include/common/texture.hpp>
#include <../include/common/controls.hpp>
#include <../include/common/objloader.hpp>

int main(void) {
    // inicializar GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // creamos la ventana
    window = glfwCreateWindow(1024, 768, "Eje de coordenadas", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    // capturamos la tecla escape
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // dibujamos solo los triangulos visibles.
    glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // creamos y compilamos el shader para los colores
    GLuint programID = LoadShaders("../shaders/Eje.vert", "../shaders/Eje.frag");

    // creamos los uniforms para el color y modelo
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint colorID = glGetUniformLocation(programID, "inputColor");

    // Cargar los OBJ de los ejes x, y, z
    std::vector<glm::vec3> verticesX, verticesY, verticesZ;
    std::vector<glm::vec2> uvsX, uvsY, uvsZ;
    std::vector<glm::vec3> normalsX, normalsY, normalsZ;

    // cargamos los ejes.
    loadOBJ("../models/ejeX.obj", verticesX, uvsX, normalsX);
    loadOBJ("../models/ejeY.obj", verticesY, uvsY, normalsY);
    loadOBJ("../models/ejeZ.obj", verticesZ, uvsZ, normalsZ);

    // Crear buffers para cada eje
    GLuint vertexbufferX, vertexbufferY, vertexbufferZ;
    glGenBuffers(1, &vertexbufferX);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferX);
    glBufferData(GL_ARRAY_BUFFER, verticesX.size() * sizeof(glm::vec3), &verticesX[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vertexbufferY);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferY);
    glBufferData(GL_ARRAY_BUFFER, verticesY.size() * sizeof(glm::vec3), &verticesY[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vertexbufferZ);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferZ);
    glBufferData(GL_ARRAY_BUFFER, verticesZ.size() * sizeof(glm::vec3), &verticesZ[0], GL_STATIC_DRAW);

    // activamos el dibujado de aristas ( desactivar para dibujar normal)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {

        // Limpiar pantalla
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Controles de cámara
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;


        glUseProgram(programID);

        // Enviar la matriz MVP
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Dibujar el eje X (Rojo)
        glUniform3f(colorID, 1.0f, 0.0f, 0.0f); // Color rojo
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferX);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, verticesX.size());
        glDisableVertexAttribArray(0);

        // Dibujar el eje Y (Verde)
        glUniform3f(colorID, 0.0f, 1.0f, 0.0f); // Color verde
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferY);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, verticesY.size());
        glDisableVertexAttribArray(0);

        // Dibujar el eje Z (Azul)
        glUniform3f(colorID, 0.0f, 0.0f, 1.0f); // Color azul
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferZ);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glDrawArrays(GL_TRIANGLES, 0, verticesZ.size());
        glDisableVertexAttribArray(0);

        // Intercambiar buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // limpiar
    glDeleteBuffers(1, &vertexbufferX);
    glDeleteBuffers(1, &vertexbufferY);
    glDeleteBuffers(1, &vertexbufferZ);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();

    return 0;
}
