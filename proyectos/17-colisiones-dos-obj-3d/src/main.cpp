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
#include <glm/gtc/type_ptr.hpp>
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
	window = glfwCreateWindow( 1024, 768, "ventana", NULL, NULL);
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
	GLuint programIDSaturno = LoadShaders( "../shaders/TransformVertexShaderMaza.glsl", "../shaders/TextureFragmentShaderMaza.glsl" );
	GLuint programIDAnillos = LoadShaders("../shaders/VertexShaderOBJ.glsl","../shaders/FragmentShaderOBJ.glsl");

	// shaders de fragmentos
	GLuint geometricProgramID = Load3Shaders( "../shaders/geometry.vert", "../shaders/geometry.frag","../shaders/geometry.geom" );
	

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programIDSaturno, "MVP");
	GLuint MatrixIDAnillos = glGetUniformLocation(programIDAnillos, "MVP");

	// Load the texture
	GLuint TexturePlaneta = loadDDS("../shaders/planeta.dds");
	GLuint TextureAnillos = loadDDS("../shaders/anillos.dds");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programIDSaturno, "myTextureSampler");
	GLuint TextureIDAnillos  = glGetUniformLocation(programIDAnillos, "TexturaAnillos");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	bool res = loadOBJ("../models/saturn.obj", vertices, uvs, normals);

	// Variables para el plano (anillos)
	std::vector<glm::vec3> verticesAnillos;
	std::vector<glm::vec2> uvsAnillos;
	std::vector<glm::vec3> normalsAnillos;  // No se usará
	bool resPlano = loadOBJ("../models/anillos.obj", verticesAnillos, uvsAnillos, normalsAnillos);

	std::vector<glm::vec3> combinedNormals = normals;
	combinedNormals.insert(combinedNormals.end(), normalsAnillos.begin(), normalsAnillos.end());

	std::vector<glm::vec3> combinedVertices = vertices;
	combinedVertices.insert(combinedVertices.end(), verticesAnillos.begin(), verticesAnillos.end());

	// buffer de normales
	GLuint Combinednormalbuffer;
	glGenBuffers(1, &Combinednormalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, Combinednormalbuffer);
	glBufferData(GL_ARRAY_BUFFER, combinedNormals.size() * sizeof(glm::vec3), &combinedNormals[0], GL_STATIC_DRAW);

	// Crear y cargar el buffer combinado de vértices
	GLuint combinedVertexBuffer;
	glGenBuffers(1, &combinedVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, combinedVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, combinedVertices.size() * sizeof(glm::vec3), &combinedVertices[0], GL_STATIC_DRAW);

	// Carga en un VBO para el plano (anillos)
	GLuint vertexbufferAnillos;
	glGenBuffers(1, &vertexbufferAnillos);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferAnillos);
	glBufferData(GL_ARRAY_BUFFER, verticesAnillos.size() * sizeof(glm::vec3), &verticesAnillos[0], GL_STATIC_DRAW);

	// Load it into a VBO para Saturno
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	// Buffer de UVs para Saturno
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	// Buffer de UVs para los anillos
	GLuint uvbufferAnillos;
	glGenBuffers(1, &uvbufferAnillos);
	glBindBuffer(GL_ARRAY_BUFFER, uvbufferAnillos);
	glBufferData(GL_ARRAY_BUFFER, uvsAnillos.size() * sizeof(glm::vec2), &uvsAnillos[0], GL_STATIC_DRAW);

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {

		// Limpiar pantalla
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// ---- Renderizar los anillos ----
		glUseProgram(programIDAnillos); // Usar el shader para los anillos

		// Calcular y enviar la matriz MVP para los anillos
		glm::mat4 ModelMatrixAnillos = glm::mat4(1.0f);  // Posicionar los anillos si lo deseas
		glm::mat4 MVPAnillos = ProjectionMatrix * ViewMatrix * ModelMatrixAnillos;
		glUniformMatrix4fv(MatrixIDAnillos, 1, GL_FALSE, &MVPAnillos[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TextureAnillos);
		glUniform1i(TextureIDAnillos, 0);

		// Enviar los vértices de los anillos
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferAnillos);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Enviar las UVs de los anillos
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbufferAnillos);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, verticesAnillos.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// ---- Renderizar Saturno ----
		glUseProgram(programIDSaturno); // Usar el shader para Saturno
		// Calcular y enviar la matriz MVP para Saturno
		glm::mat4 ModelMatrixSaturno = glm::mat4(1.0f);  // Matriz de modelo para Saturno
		glm::mat4 MVPSaturno = ProjectionMatrix * ViewMatrix * ModelMatrixSaturno;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPSaturno[0][0]);
				
		// Enviar la textura de Saturno
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TexturePlaneta);
		glUniform1i(TextureID, 0);

		// Enviar los vértices de Saturno
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		
		// Enviar las UVs de Saturno
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Dibujar Saturno
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		//---- renderizar normales----
		glUseProgram(geometricProgramID);
		// Pass transformation matrices to the shader
        glUniformMatrix4fv(glGetUniformLocation(geometricProgramID, "model"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(geometricProgramID, "view"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(geometricProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

		// pasamos vertices y luego normales
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, combinedVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, Combinednormalbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // dibujamos las normales
        glDrawArrays(GL_TRIANGLES, 0, combinedVertices.size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

		// Intercambiar buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programIDSaturno);
	glDeleteTextures(1, &TexturePlaneta);
	
	glDeleteBuffers(1, &vertexbufferAnillos);
	glDeleteBuffers(1, &uvbufferAnillos);
	glDeleteProgram(programIDAnillos);
	glDeleteTextures(1, &TextureAnillos);

	glDeleteBuffers(1,&Combinednormalbuffer);
	glDeleteBuffers(1,&combinedVertexBuffer);
	
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
