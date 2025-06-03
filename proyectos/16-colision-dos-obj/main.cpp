// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>

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


const float orbitRadiusSaturno = 10.0f; // Radio de la órbita para Saturno
const float orbitRadiusUrano = 8.0f;   // Radio de la órbita para Urano

const float rotationAngleSaturno = 0.002f; // Rotación por frame para Saturno
const float rotationAngleUrano = -0.003f;  // Rotación por frame para Urano

// Umbral para detectar colisión
const float collisionThreshold = 2.0f; // Ajusta este valor según el tamaño de tus objetos

// Función para calcular la distancia entre dos puntos 3D
float distance(const glm::vec3& p1, const glm::vec3& p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}

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
	GLuint programIDUrano = LoadShaders("../shaders/VertexPlano.glsl","../shaders/FragmentPlano.glsl");

	// Get a handle for our "MVP" uniform
	GLuint MatrixIDSaturno = glGetUniformLocation(programIDSaturno, "MVP");
	GLuint MatrixIDUrano = glGetUniformLocation(programIDUrano, "MVP");

	// Textura de Saturno
	GLuint TextureSaturno = loadDDS("../shaders/Saturn2_Saturn_Metallic_1.dds");
	GLuint TextureIDSaturno  = glGetUniformLocation(programIDSaturno, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> verticesSaturno;
	std::vector<glm::vec2> uvsSaturno;
	std::vector<glm::vec3> normalsSaturno; // Won't be used at the moment.
	bool saturno = loadOBJ("../models/cubo.obj", verticesSaturno, uvsSaturno, normalsSaturno);

		// Variables para urano
	std::vector<glm::vec3> verticesUrano;
	std::vector<glm::vec2> uvsUrano;  
	std::vector<glm::vec3> normalsUrano; 
	bool urano = loadOBJ("../models/piramide.obj", verticesUrano, uvsUrano, normalsUrano);

	// Carga en un VBO para el plano
	GLuint vertexbufferUrano,vertexbufferSaturno,lineBuffer;

	glGenBuffers(1, &vertexbufferUrano);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferUrano);
	glBufferData(GL_ARRAY_BUFFER, verticesUrano.size() * sizeof(glm::vec3), &verticesUrano[0], GL_STATIC_DRAW);

	glGenBuffers(1, &vertexbufferSaturno);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferSaturno);
	glBufferData(GL_ARRAY_BUFFER, verticesSaturno.size() * sizeof(glm::vec3), &verticesSaturno[0], GL_STATIC_DRAW);

	glGenBuffers(1, &lineBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    float angleSaturno = 0.0f;
    float angleUrano = 0.0f;
	float scaleFactor = 0.5f;

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {

		// Limpiar pantalla
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Update rotation angles
        angleSaturno += rotationAngleSaturno;
        angleUrano += rotationAngleUrano;

		// ---- Renderizar el urano ----
		glUseProgram(programIDUrano); 
		// Matriz de escalado
		glm::mat4 ScaleMatrixUrano = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		// Matriz de traslación (desplazando 5 unidades en el eje x)
		glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f));
		// Matriz de traslación en la órbita
		glm::mat4 OrbitMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cos(angleUrano) * orbitRadiusUrano, 0.0f, sin(angleUrano) * orbitRadiusUrano));
		// Combina las transformaciones
		glm::mat4 ModelMatrixUrano = TranslationMatrix * OrbitMatrix * ScaleMatrixUrano;
        glm::mat4 MVPUrano = ProjectionMatrix * ViewMatrix * ModelMatrixUrano;
        glUniformMatrix4fv(MatrixIDUrano, 1, GL_FALSE, &MVPUrano[0][0]);

		// Enviar los vértices de urano
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferUrano);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, verticesUrano.size());
		glDisableVertexAttribArray(0);

		// ---- Renderizar la saturno ----
		glUseProgram(programIDSaturno); 
		// Matriz de escalado para Saturno
		glm::mat4 ScaleMatrixSaturno = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor, scaleFactor, scaleFactor));
		// Matriz de traslación (desplazando unidades en el eje )
		glm::mat4 TranslationMatrixSaturno = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		// Matriz de traslación en la órbita
		glm::mat4 OrbitMatrixSaturno = glm::translate(glm::mat4(1.0f), glm::vec3(cos(angleSaturno) * orbitRadiusSaturno, 0.0f, sin(angleSaturno) * orbitRadiusSaturno));
		// Combina las transformaciones
		glm::mat4 ModelMatrixSaturno = TranslationMatrixSaturno * OrbitMatrixSaturno * ScaleMatrixSaturno;
		// matriz mvp
        glm::mat4 MVPSaturno = ProjectionMatrix * ViewMatrix * ModelMatrixSaturno;
        glUniformMatrix4fv(MatrixIDSaturno, 1, GL_FALSE, &MVPSaturno[0][0]);

		// Enviar los vértices 
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbufferSaturno);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// Dibujar saturno
		glDrawArrays(GL_TRIANGLES, 0, verticesSaturno.size());
		glDisableVertexAttribArray(0);
	

		// Definir las posiciones de los objetos en el espacio
		glm::vec3 posicionSaturno = glm::vec3(cos(angleSaturno) * orbitRadiusSaturno, 0.0f, sin(angleSaturno) * orbitRadiusSaturno);
		glm::vec3 posicionUrano = glm::vec3(3.0f, 0.0f, 0.0f) + glm::vec3(cos(angleUrano) * orbitRadiusUrano, 0.0f, sin(angleUrano) * orbitRadiusUrano);

		// Actualizar los datos de los vértices de la línea
		std::vector<glm::vec3> lineVertices = { posicionSaturno, posicionUrano };
		glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, lineVertices.size() * sizeof(glm::vec3), &lineVertices[0]);

		// Restablecer la transformación de la línea a una identidad
		glm::mat4 ModelMatrixLine = glm::mat4(1.0f);
		glm::mat4 MVPLine = ProjectionMatrix * ViewMatrix * ModelMatrixLine;

		// Usar uno de los programas de shader para la línea (puedes usar el shader de Saturno)
		glUseProgram(programIDSaturno);
		glUniformMatrix4fv(MatrixIDSaturno, 1, GL_FALSE, &MVPLine[0][0]);  // Asignar la matriz MVP para la línea

		// Dibujar la línea
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_LINES, 0, 2);  // Dibujar la línea entre los dos puntos
		glDisableVertexAttribArray(0);

		// Calcular la distancia entre Saturno y Urano
        float distancia = distance(posicionSaturno, posicionUrano);

        // Verificar si están lo suficientemente cerca para considerarse una "colisión"
        if (distancia < collisionThreshold) {
            printf("Las orbitas se han intersectado.\n");
        }

		// Intercambiar buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbufferSaturno);
	glDeleteBuffers(1, &vertexbufferUrano);
	glDeleteBuffers(1, &lineBuffer);
	glDeleteProgram(programIDSaturno);
	glDeleteProgram(programIDUrano);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

