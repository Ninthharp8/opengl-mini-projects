#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <chrono>

// Función para compilar los shaders
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Verificar errores de compilación
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configurar la versión de OpenGL a utilizar
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Crear una ventana de 800x800 píxeles
    GLFWwindow* window = glfwCreateWindow(800, 800, "ventana xd", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Introducir la ventana en el contexto actual
    glfwMakeContextCurrent(window);

    // Cargar GLAD para configurar OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Especificar el viewport de OpenGL
    glViewport(0, 0, 800, 800);

    // Compilar y enlazar los shaders
    const char* fragmentShaderSource = R"(
        #version 460 core
        uniform vec2 u_resolution;
        uniform float u_time;
        out vec4 FragColor;

        float box(in vec2 _st, in vec2 _size) {
            _size = vec2(0.5) - _size * 0.5;
            vec2 uv = smoothstep(_size, _size + vec2(0.001), _st);
            uv *= smoothstep(_size, _size + vec2(0.001), vec2(1.0) - _st);
            return uv.x * uv.y;
        }

        float cross(in vec2 _st, float _size) {
            return box(_st, vec2(_size, _size / 4.0)) + box(_st, vec2(_size / 4.0, _size));
        }

        void main() {
            vec2 st = gl_FragCoord.xy / u_resolution.xy;
            vec3 color = vec3(0.0);
            vec2 translate = vec2(cos(u_time), sin(u_time));
            st += translate * 0.35;
            color += vec3(cross(st, 0.25));
            FragColor = vec4(color, 1.0);
        }
    )";

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(fragmentShader);

    // Verificar errores de enlazado
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Configurar el bucle de renderizado
    while (!glfwWindowShouldClose(window)) {
        // Establecer el color de fondo
        glClear(GL_COLOR_BUFFER_BIT);

        // Usar el programa de shaders
        glUseProgram(shaderProgram);

        // Pasar los valores de uniform
        float timeValue = glfwGetTime();
        glUniform1f(glGetUniformLocation(shaderProgram, "u_time"), timeValue);
        glUniform2f(glGetUniformLocation(shaderProgram, "u_resolution"), 800.0f, 800.0f);

        // Dibujar un cuadrado que cubra toda la pantalla
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Intercambiar los buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpiar recursos
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
