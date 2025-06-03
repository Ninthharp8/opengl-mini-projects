// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLAD
#include <glad/glad.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

// Include custom headers (ensure correct paths)
#include "../include/common/shader.hpp"
#include "../include/common/texture.hpp"
#include "../include/common/controls.hpp"
#include "../include/common/objloader.hpp"

int main(void) {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);  // Antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "ventana", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.\n");
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("../shaders/VertexShader.glsl", "../shaders/FragmentShader.glsl");

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    GLuint  InstancedID = glGetUniformLocation(programID, "Transformation");

    // Define your vertex data
    glm::vec2 vertices[] =
    {
        glm::vec2(+1.f, +0.f),
        glm::vec2(+0.f, +1.f),
        glm::vec2(-1.f, +0.f),
        glm::vec2(+0.f, -1.f),
    };

    // Colors
    GLubyte colors[] = 
    {
        0, 255, 0,
        0, 255, 0,
        0, 255, 0,
        0, 255, 0
    };

    // Indices
    unsigned int indices[] =
    { 
        0, 1, 3,
        1, 2, 3 
    };

    // Offset data for instancing
    glm::vec2 centerOffset[4] = {
        glm::vec2(-0.5f, 0.5f), glm::vec2(0.5f, 0.5f),
        glm::vec2(0.5f, -0.5f), glm::vec2(-0.5f, -0.5f),
    };

    unsigned int EBO, VAO, positionVBO, colorVBO, centerOffsetVBO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenBuffers(1, &positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glGenBuffers(1, &centerOffsetVBO);
    glBindBuffer(GL_ARRAY_BUFFER, centerOffsetVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(centerOffset), centerOffset, GL_STATIC_DRAW);

    // Setup VAO
    glBindVertexArray(VAO);
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, centerOffsetVBO);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribDivisor(2, 1);  // Use instancing for offsets
    }
    glBindVertexArray(0);

    // Matrices
    glm::mat4 resizeMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.25f, 0.25f, 1.f));
    float startTime = glfwGetTime();
    glm::mat4 model[4];

    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        // Calculate time
        float currentTime = glfwGetTime();
        float dt = currentTime - startTime;

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Update model transformation
        model[0] = glm::rotate(glm::mat4(1.f), -1.75f * dt, glm::vec3(0.f, 0.f, 1.f));

        glBindVertexArray(VAO);
        {
            glm::mat4 modelToWorld = resizeMatrix * model[0];
            glUseProgram(programID);
            
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, glm::value_ptr(MVP)); // mi camara
            glUniformMatrix4fv(InstancedID, 1, GL_FALSE, glm::value_ptr(modelToWorld));
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 4);
        }
        glBindVertexArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup VBO and shader
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &positionVBO);
    glDeleteBuffers(1, &colorVBO);
    glDeleteBuffers(1, &centerOffsetVBO);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VAO);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
