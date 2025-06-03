// 2 triangulos que aparentan ser 3. 

#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<cmath>

// vertex shader source code
const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// fragment shader source code
const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
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

     GLfloat vertices[] =  
    {
        -0.5f, -0.5f * float(sqrt(3)) / 3 , 0.0f, 
        0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,   
        0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,
        -0.5f / 2,  0.5f * float(sqrt(3)) / 6, 0.0f ,
        0.5f / 2,  0.5f * float(sqrt(3)) / 6, 0.0f ,
        0.0f ,  -0.5f * float(sqrt(3)) / 3, 0.0f ,
    };

    GLuint indices[] = {
       0,3,5,
       3,2,4,
       5,4,1 
    };


     // Create reference containers for the Vertex Array Object and the Vertex Buffer Object
    GLuint VAO, VBO, EBO;

    // Generate the VAO and VBO with only 1 object each
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glGenBuffers(1, &EBO);

    // Make the VAO the current Vertex Array Object by binding it
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    


    // Configure the vertex attribute so that OpenGL knows how to read the VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Enable the vertex attribute so that OpenGL knows to use it 
    glEnableVertexAttribArray(0);

    // Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

      // Main while loop
    while (!glfwWindowShouldClose(window))
    {
        // Specify the color of the background 
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // Clean the back buffer and assign the new color to it 
        glClear(GL_COLOR_BUFFER_BIT);
        // Tell OpenGL which shader program we want to use 
        glUseProgram(shaderProgram);
        // Bind the VAO so OpenGL knows to use it 
        glBindVertexArray(VAO);
        // Draw the triangle using the GL_TRIANGLES primitive
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES,9,GL_UNSIGNED_INT,0);
        // Swap the back buffer with the front buffer 
        glfwSwapBuffers(window);
        // Take care of all GLFW events 
        glfwPollEvents();
    }

    // Properly clean up resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();



}