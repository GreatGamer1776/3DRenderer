#include "utils/render.hpp"
#include "utils/shader.hpp"
#include "shapes/Cube.h"
#include <glad/gl.h>
#include <ext/matrix_float4x4.hpp>
#include <ext.hpp>

int main(){

	//Start the subsystem and make the window. Also make a scope for the window so the window destroys after the main loop closes
	Render::startRender();

	//Set opengl attributes
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); //Set major and minor version of opengl. These versions dont work on all systems tho.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); //For each color make an 8 bit size buffer
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	//Create the window with resizeable flag
	SDL_Window* window = SDL_CreateWindow("3D Renderer", Render::windowWidth, Render::windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (!window) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window was not created correctly!");
		return 1;
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer was not created correctly!");
		return 1;
	}

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
	printf("GL %d.%d\n", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	Render::isRunning = true;

	//Having a vertex buffer for 3 vectors for a triangle, X/Y/Z
	//static const GLfloat vertexBufferData[] = {
	//	-0.5f, -0.5f, 0.0f, //First vertice at the bottom left
	//	0.5f, -0.5f, 0.0f, //Second vertice at the bottom right
	//	0.0f,  0.5f, 0.0f, //Third vertice at the middle top of the screen
	//};

	//First, create the variable for the vertex buffer then put turn that variable into a vertex buffer
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	//This will talk about our vertex buffer??? im not sure what this means
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//Give the vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube::cubeVerticies), Cube::cubeVerticies, GL_STATIC_DRAW);

	//Make a buffer and bind data to it
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Cube::cubeColorData), Cube::cubeColorData, GL_STATIC_DRAW);

	//Compile shader
	GLuint shaderID = Shader::LoadShaders("shaders/testVertexShader.glsl", "shaders/testFragmentShader.glsl");


	//Now for persepctive
	// Projection matrix: 45° Field of View, 4:3 ratio, display range: 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)Render::windowWidth / (float)Render::windowHeight, 0.1f, 400.0f);

	// Or, for an ortho camera:
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix: an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection: multiplication of our 3 matrices
	glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// Get a handle for our "MVP" uniform
	// Only during the initialisation
	GLuint MatrixID = glGetUniformLocation(shaderID, "MVP");

	// Send our transformation to the currently bound shader, in the "MVP" uniform
	// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);


	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	while (Render::isRunning) {
		//Event Polling
		std::optional<SDL_Event> event;
		while (event = Render::pollEvent()) //Make sure to poll ALL events
		{
			switch (event->type) {
			case SDL_EventType::SDL_EVENT_QUIT: //check if it the close window event
					Render::isRunning = false;
					break;
			}
		}

		//Render stuff
		// 
		// Clear the screen and set draw color
		glClearColor(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f);
		// Enable depth test
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Draw stuff here
		//Drawing the triangle
		//Use shader
		try {
			glUseProgram(shaderID);
		}
		catch(std::exception &exception){
			SDL_LogCritical(SDL_LogCategory::SDL_LOG_CATEGORY_ERROR, exception.what());
		}
		
		//Enables vertex attribute array
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); //Bind a named buffer object to a specific buffer
		glVertexAttribPointer(
			0,                  //Set attribute to 0, no reason but make sure to match the layout in the shader
			3,                  //Number of components
			GL_FLOAT,           //Specifies the data type of each item in the array
			GL_FALSE,           //Should we normalize it?
			0,                  //Offset between consecutive vertex attributes
			(void*)0            //Offset of the first component in the array
		);
		//Finally draw the square
		glDrawArrays(GL_TRIANGLES, 0, 12*3); //Starting from vertex 0 draw the square

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glDisableVertexAttribArray(0); //Disables the array

		//Update the screen
		SDL_GL_SwapWindow(window);


	}
	
	//End the subsystem and destroy renderer and window
	SDL_GL_DestroyContext(glContext);
	SDL_DestroyWindow(window);
	Render::endRender();

	return 0;
}