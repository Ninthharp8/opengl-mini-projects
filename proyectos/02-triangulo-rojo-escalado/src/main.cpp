#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// vertex shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform float scale;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos * scale, 1.0);\n"
"}\0";

// fragment shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n" 
"void main()\n"
"{\n"
"    FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
"}\n\0";


int main()
{
    // Initialize GLFW
    glfwInit();

    // Tell GLFW what version of OpenGL we are using 
    // In this case we are using OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);    

    // Tell GLFW we are using the core profile 
    // so that means we only have the modern functions
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFWwindow object of 800 by 800 pixels, naming it "ventana xd"
    GLFWwindow* window = glfwCreateWindow(800, 800, "ventana xd", NULL, NULL);

    // Error check if the window fails to create 
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Introduce the window into the current context
    glfwMakeContextCurrent(window);

    // Load GLAD so it configures OpenGL
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Specify the viewport of OpenGL
    glViewport(0, 0, 800, 800);

    // Create vertex shader object and get its reference 
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Attach vertex shader source to vertex shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Create fragment shader object and get its reference
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Attach fragment shader source to the fragment shader object 
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create shader program object and get its reference
    GLuint shaderProgram = glCreateProgram();

    // Attach the vertex and fragment shaders to the shader program 
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // Link all the shaders together into the shader program
    glLinkProgram(shaderProgram);

    // Delete the now useless vertex and fragment shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertices coordinates 
    GLfloat vertices[] =  
    {
        -0.5f, -0.5f , 0.0f, // lower left corner
        0.5f, -0.5f, 0.0f,  // lower right corner 
        0.0f,  0.5f, 0.0f   // upper corner
    };

    // Create reference containers for the Vertex Array Object and the Vertex Buffer Object
    GLuint VAO, VBO;

    // Generate the VAO and VBO with only 1 object each
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Make the VAO the current Vertex Array Object by binding it
    glBindVertexArray(VAO);

    // Bind the VBO specifying it's a GL_ARRAY_BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Introduce the vertices into the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configure the vertex attribute so that OpenGL knows how to read the VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Enable the vertex attribute so that OpenGL knows to use it 
    glEnableVertexAttribArray(0);

    // Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // // Set the scale valu
     float scaleValue = 1.5f;

    // Main while loop
    while (!glfwWindowShouldClose(window))
    {
        // Specify the color of the background 
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // Clean the back buffer and assign the new color to it 
        glClear(GL_COLOR_BUFFER_BIT);
        // Tell OpenGL which shader program we want to use 
        glUseProgram(shaderProgram);

        // Set the uniform value for scale
        GLint scaleLoc = glGetUniformLocation(shaderProgram,"scale");
        glUniform1f(scaleLoc,scaleValue);

        // set the inform value for te color
        GLint colorLoc = glGetUniformLocation(shaderProgram,"ourColor");
        glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0);

        // Bind the VAO so OpenGL knows to use it 
        glBindVertexArray(VAO);
        // Draw the triangle using the GL_TRIANGLES primitive
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // Swap the back buffer with the front buffer 
        glfwSwapBuffers(window);
        // Take care of all GLFW events 
        glfwPollEvents();
    }

    // Properly clean up resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Terminate GLFW
    glfwTerminate();

    return 0;
}
