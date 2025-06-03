// trifuerza que respira

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include <chrono> 

// Código fuente del shader de vértices
const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"

"out vec3 color;\n"
"uniform float scale;\n"

"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x + aPos.x * scale , aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);\n"
"    color = aColor;\n"
"}\0";

// Código fuente del shader de fragmentos
const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"in vec3 color;\n"

"void main()\n"
"{\n"
"    FragColor = vec4(color, 1.0f);\n"
"}\n\0";

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
 

int main()
{
    // Inicializar GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configurar la versión de OpenGL a utilizar
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Crear una ventana de 800x800 píxeles
    GLFWwindow* window = glfwCreateWindow(800, 800, "ventana xd", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwSetKeyCallback(window, key_callback);
    
    // Introducir la ventana en el contexto actual
    glfwMakeContextCurrent(window);

    // Cargar GLAD para configurar OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Especificar el viewport de OpenGL
    glViewport(0, 0, 800, 800);

    // Crear el objeto del shader de vértices y adjuntar el código fuente
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Crear el objeto del shader de fragmentos y adjuntar el código fuente
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Crear el programa de shader y vincular los shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Eliminar los objetos de shader que ya no se utilizan
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Definir los vértices y colores de los triángulos
    GLfloat vertices[] =  
    {
        -0.5f, -0.5f * float(sqrt(3)) / 3 , 0.0f,     0.8f, 0.3f, 0.02f,
        0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,        0.8f, 0.3f, 0.02f,
        0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f, 0.32f,
        -0.5f / 2,  0.5f * float(sqrt(3)) / 6, 0.0f ,   0.9f,0.45f,0.17f,
        0.5f / 2,  0.5f * float(sqrt(3)) / 6, 0.0f ,    0.9f,0.45f,0.17f,
        0.0f ,  -0.5f * float(sqrt(3)) / 3, 0.0f ,      0.8f,0.3f,0.02f
    };

    GLuint indices[] = {
       0, 3, 5,
       3, 2, 4,
       5, 4, 1 
    };

    // Crear el VAO, VBO y EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Configurar el VAO
    glBindVertexArray(VAO);

    // Configurar el EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Configurar el VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configurar los atributos del vértice
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Desvincular el VAO y el VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Obtener el ID del uniforme "scale" en el shader
    GLuint uniID = glGetUniformLocation(shaderProgram, "scale");

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
        glUniform1f(uniID, scaleValue);

        // Dibujar los triángulos
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

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