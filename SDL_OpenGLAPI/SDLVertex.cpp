#include "SDL_OpenGLAPI.h"

extern GAMESTATE STATE;


//without shader file, default vertex shader in OpenGL?
void SDLVertex()
{
	SDL_Window* window = SDL_CreateWindow("Vertex Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 300, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Glew init failed" << std::endl;
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glClearColor(0, 0, 0, 1);

	//vertex
	GLuint vbo = 0;

	//bind buffer
	glGenBuffers(1, &vbo);
	//NDC coordinate (-1, 1)
	float vertice[6] = { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f };
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);
	//bind vertex attrib
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(0);

	while (STATE != GAMESTATE::EXIT)
	{
		ProcessInput();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//draw
		glDrawArrays(GL_TRIANGLES, 0, 3);
		SDL_GL_SwapWindow(window);

	}

	glDeleteBuffers(1, &vbo);
	
	SDL_GL_DeleteContext(context);

}