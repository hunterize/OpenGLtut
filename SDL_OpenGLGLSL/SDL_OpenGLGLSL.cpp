
#include "SDL_OpenGLGLSL.h"

bool isRunning = true;

int main(int argc, char** argv)
{

	BasicShaderProgram();

	return 1;
}

void ProcessInput()
{

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				isRunning = false;
				break;
			}
			break;

		default:
			break;
		}
	}
}

