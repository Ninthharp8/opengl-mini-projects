// tetraedro que gira.

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <../include/configuracion/opengl.h>
#include <../include/configuracion/shaders.h>
#include <../include/configuracion/buffer.h>


int main()
{
    // Inicializar GLFW
    GLFWwindow* window = initWindow(800, 800, "ventana xd");
        if (!window) return -1;
     // Cargar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Configuración de shaders
    GLuint shaderProgram = createShaderProgram("../shaders/VertexShader.glsl", "../shaders/FragmentShader.glsl");

    //camara

    glm::mat4 model = glm::mat4(1.0); 
    glm::mat4 view = glm::mat4(1.0); 
    glm::mat4 proj = glm::mat4(1.0); 

       // Definir los vértices y colores del tetraedro
    GLfloat vertices[] = {
        // Coordenadas           // Colores
        1.0f,  1.0f,  1.0f,    0.8f, 0.3f, 0.02f,  // Vértice 0
        -1.0f, -1.0f,  1.0f,    0.8f, 0.3f, 0.02f,  // Vértice 1
        -1.0f,  1.0f, -1.0f,    0.9f, 0.45f, 0.17f, // Vértice 2
        1.0f, -1.0f, -1.0f,    1.0f, 0.6f, 0.32f   // Vértice 3
    };

    // Definir los índices para formar las caras del tetraedro
    GLuint indices[] = {
        0, 1, 2,  // Cara 1
        0, 1, 3,  // Cara 2
        0, 2, 3,  // Cara 3
        1, 2, 3   // Cara 4
    };

    GLuint VAO, VBO, EBO;
    createBuffers(VAO, VBO, EBO, vertices, indices, sizeof(vertices), sizeof(indices));

    // Obtener el ID del uniforme "scale" en el shader
    GLuint uniID = glGetUniformLocation(shaderProgram, "scale");

    
    GLuint modelLoc = glGetUniformLocation(shaderProgram,"model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram,"view");
    GLuint projLoc = glGetUniformLocation(shaderProgram,"proj");

    view = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f),  // Posición de la cámara
                   glm::vec3(0.0f, 0.0f, 0.0f),  // Hacia dónde mira la cámara
                   glm::vec3(0.0f, 1.0f, 0.0f)); // Vector up

   // Cambiar la matriz de proyección a perspectiva
    proj = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);

    // Bucle principal
    while (!glfwWindowShouldClose(window))
    {
        // Obtener el tiempo transcurrido
        float timeValue = glfwGetTime();

        // Oscilar el valor de "scale" entre 0.0 y 1.0
        float scaleValue = sin(timeValue) / 2.0f + 0.5f;

        // Limpiar el buffer de color
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Usar el programa de shaders
        glUseProgram(shaderProgram);

        // Pasar el valor de "scale" al shader

        model = glm::rotate(glm::mat4(1.0f), glm::radians(timeValue * 50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
        glUniformMatrix4fv(projLoc,1,GL_FALSE,glm::value_ptr(proj));
        glUniform1f(uniID, scaleValue);

        
        // Dibujar los triángulos
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // cambiar entre GL_line o GL_FILL
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        // Intercambiar los buffers y procesar eventos
        glfwSwapBuffers(window);
        glfwPollEvents();

       
    }

    // Limpiar recursos
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // Terminar GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}