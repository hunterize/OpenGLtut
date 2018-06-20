
#include "SDL_OpenGLAPI.h"

extern GAMESTATE STATE;

void SDLVertexImmediate()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	//init window
	SDL_Window* window = SDL_CreateWindow("Vertex Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 300, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//init glew
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "Glew init failed" << std::endl;
		exit(1);
	}

	//set opengl attribute
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glClearColor(0, 0, 0, 1);

	while (STATE != GAMESTATE::EXIT)
	{
		ProcessInput();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//immediate mode vertex

		glBegin(GL_TRIANGLES);

		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-1, -1);
		glVertex2f(0, -1);
		glVertex2f(0, 1);


		glEnd();
		
		
		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
}