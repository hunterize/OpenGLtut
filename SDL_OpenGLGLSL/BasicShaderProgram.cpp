#include "SDL_OpenGLGLSL.h"

extern bool isRunning;

void BasicShaderProgram()
{
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;
	//bool isRunning = true;

	//initialize shader instance
	CShader shader;

	int screenWidth = 800;
	int screenHeight = 600;

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("Basic GLSL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);



	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "Glew init error" << std::endl;
		exit(0);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


	//load vertex shader and fragment shader
	shader.AttachShader("Shaders/BasicVertexShader.vert", "Shaders/BasicFragmentShader.frag");

	//set vertex
	GLuint vbo = 0;
	GLuint vao = 0;

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	GLfloat vertices[] = {
		-1.0f, -1.0f, 1.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0, 1.0, 0.0,
		1.0f, -1.0f, 0.0, 0.0, 1.0
	};

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//location = 0 in vertex shader
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//location = 1 in vertex shader 
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//end of set vertex

	while (isRunning)
	{
		ProcessInput();

		//use shader
		shader.Use();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);

		//draw
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

		//unuse shader
		shader.Unuse();

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
}