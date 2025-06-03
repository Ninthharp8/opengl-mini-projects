// un triangulo que respira con textura y colores

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// Código del shader de vértices
const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec3 color;\n"
"out vec2 texCoord;\n"
"uniform float scale;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);\n"
"    color = aColor;\n"
"    texCoord = aTexCoord;\n"
"}\0";

// Código del shader de fragmento
const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"in vec3 color;\n"
"in vec2 texCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"    FragColor = texture(ourTexture, texCoord) * vec4(color, 1.0f);\n"
"}\n\0";

// Callback para la tecla de escape
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main()
{
    // Inicialización de GLFW
    if (!glfwInit()) {
        std::cout << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

    // Configuración de la versión de OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creación de la ventana GLFW
    GLFWwindow* window = glfwCreateWindow(800, 800, "ventana xd", NULL, NULL);
    if (window == NULL) {
        std::cout << "No se pudo crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Carga de GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Error al cargar GLAD" << std::endl;
        return -1;
    }

    // Configuración del viewport de OpenGL
    glViewport(0, 0, 800, 800);
    glfwSetKeyCallback(window, key_callback);

    // Compilación y enlace de shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Datos de los vértices y coordenadas de textura
    GLfloat vertices[] = {
        // Posiciones         // Colores          // Coordenadas de textura
        -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,   1.f, 0.f, 0.f,  0.0f, 0.0f, // Esquina inferior izquierda
         0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,   0.f, 1.f, 0.f,  1.0f, 0.0f, // Esquina inferior derecha
         0.5f,  0.5f * float(sqrt(3)) / 3, 0.0f,   0.f, 0.f, 1.f,  1.0f, 1.0f  // Esquina superior
    };

    // Configuración de VAO y VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configuración de atributos de vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Cargar textura
    int width, height, nrChannels;
    unsigned char* data = stbi_load("../shaders/Sheader.jpg", &width, &height, &nrChannels, 0);
    GLuint texture;
    if (data) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Configurar los parámetros de la textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Cargar la imagen en OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }

    // Obtener el identificador del uniforme
    GLuint uniID = glGetUniformLocation(shaderProgram, "scale");

    // Bucle principal
    while (!glfwWindowShouldClose(window))
    {
        float timeValue = glfwGetTime();
        float scaleValue = sin(timeValue) / 2.0f + 0.5f;

        // Configuración del color de fondo
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Usar el shader y actualizar el uniforme
        glUseProgram(shaderProgram);
        glUniform1f(uniID, scaleValue);

        // Vincular textura y renderizar
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Intercambio de buffers y procesamiento de eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Liberación de recursos
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &texture);
    glDeleteProgram(shaderProgram);

    // Terminación de GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}
