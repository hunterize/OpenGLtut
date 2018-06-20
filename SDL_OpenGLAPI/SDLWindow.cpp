#include "SDL_OpenGLAPI.h"

void SDLWindow()
{
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;
	bool isRunning = true;

	int with, height;

	with = 400;
	height = 300;

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("Test Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, with, height, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Event event;
	while (isRunning)
	{
		while(SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				std::cout << event.key.keysym.sym << std::endl;
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					isRunning = false;
					break;
				}
				break;
			}
		}

		SDL_GL_SwapWindow(window);
	}
	
}