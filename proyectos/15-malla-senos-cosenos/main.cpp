// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath> // Para sinf y cosf

// Include GLAD
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

#define F_PI 3.14159265358979323846f

std::vector<float> generateSineMeshVertices(float amplitude, float frequency, float offsetY, float length, int numSegments) {
    std::vector<float> vertices;
    float deltaX = length / numSegments;
    
    for (int i = 0; i <= numSegments; ++i) {
        float x = i * deltaX;
        float y1 = amplitude * sinf(frequency * x) + offsetY;
        float y2 = amplitude * sinf(frequency * x) - offsetY;
        
        // Vértices de la primera curva (superior)
        vertices.push_back(x);  // x
        vertices.push_back(y1); // y
        vertices.push_back(0.0f); // z

        // Vértices de la segunda curva (inferior)
        vertices.push_back(x);  // x
        vertices.push_back(y2); // y
        vertices.push_back(0.0f); // z
    }

    return vertices;
}

int main(void)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "ventana", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("../shaders/Vertex.glsl", "../shaders/Fragment.glsl");

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    // Generar vértices para la malla de la función seno
    //std::vector<float> sineMeshVertices = generateSineMeshVertices(.5f, 2.0f, 0.7f, 4.f, 20);
    //std::vector<float> sineMeshVertices = generateSineMeshVertices(0.5f, 2.0f, 0.2f, 10.0f, 500); 
    //std::vector<float> sineMeshVertices = generateSineMeshVertices(0.5f, 2.0f, 0.2f, 2.0f, 100);
    std::vector<float> sineMeshVertices = generateSineMeshVertices(0.5f, 1.0f, .7f, 3.0f, 6);

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sineMeshVertices.size() * sizeof(float), &sineMeshVertices[0], GL_STATIC_DRAW);

    // Get a handle for our "vertexColor" uniform in the fragment shader
    GLuint colorID = glGetUniformLocation(programID, "vertexColor");

    // Asigna un color, por ejemplo, rojo
    glm::vec3 color(1.0f, 0.0f, 0.0f);
    glUniform3fv(colorID, 1, &color[0]);

     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {

        // Limpiar pantalla
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use the shader program
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // 1st attribute buffer: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

       // Dibujar la malla utilizando GL_TRIANGLE_STRIP
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sineMeshVertices.size() / 3);


        glDisableVertexAttribArray(0);

        // Intercambiar buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
