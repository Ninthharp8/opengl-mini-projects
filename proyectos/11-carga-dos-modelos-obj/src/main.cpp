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


int main( void )
{
	// Initialize GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "carga de dos modelos 3d", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

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
	GLuint programID = LoadShaders( "../shaders/TransformVertexShaderMaza.glsl", "../shaders/TextureFragmentShaderMaza.glsl" );

	GLuint plano = LoadShaders("../shaders/VertexPlano.glsl","../shaders/FragmentPlano.glsl");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint MatrixIDPlano = glGetUniformLocation(plano, "MVP");

	// Load the texture
	GLuint Texture = loadDDS("../shaders/texture.dds");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("../models/plano.obj", vertices, uvs, normals);

		// Variables para el plano
	std::vector<glm::vec3> verticesPlano;
	std::vector<glm::vec2> uvsPlano;  // No se usará
	std::vector<glm::vec3> normalsPlano;  // No se usará
	bool resPlano = loadOBJ("../models/cubo.obj", verticesPlano, uvsPlano, normalsPlano);

	// Carga en un VBO para el plano
	GLuint vertexbufferPlano;
	glGenBuffers(1, &vertexbufferPlano);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferPlano);
	glBufferData(GL_ARRAY_BUFFER, verticesPlano.size() * sizeof(glm::vec3), &verticesPlano[0], GL_STATIC_DRAW);

	// Load it into a VBO para la maza
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {

		// Limpiar pantalla
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;


		// ---- Renderizar el plano ----
		glUseProgram(plano); // Usar el shader para el plano (sin textura)


		// Calcular y enviar la matriz MVP para el plano
		glm::mat4 ModelMatrixPlano = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));  // Posicionar el plano si lo deseas
		glm::mat4 MVPPlano = ProjectionMatrix * ViewMatrix * ModelMatrixPlano;
		glUniformMatrix4fv(MatrixIDPlano, 1, GL_FALSE, &MVPPlano[0][0]);

		// Enviar los vértices del plano
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferPlano);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, verticesPlano.size());
		glDisableVertexAttribArray(0);

		// ---- Renderizar la maza ----
		glUseProgram(programID); // Usar el shader para la maza (con textura)
		// Calcular y enviar la matriz MVP para la maza
		glm::mat4 ModelMatrixMaza = glm::mat4(1.0f);  // Matriz de modelo para la maza
		glm::mat4 MVPMaza = ProjectionMatrix * ViewMatrix * ModelMatrixMaza;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPMaza[0][0]);
				
		// Enviar la textura de la maza
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);

		// Enviar los vértices de la maza
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		// Enviar las UVs de la maza
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Dibujar la maza
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Intercambiar buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

