#include "SDL_OpenGLAPI.h"



GAMESTATE STATE = GAMESTATE::RUNNING;

int main(int argc, char** argv)
{
	//SDLWindow();

	//SDLVertexImmediate();

	//SDLVertex();

	//SDLVAO();

	//SDLEBO();

	SDLTexture();


	return 1;
}

void Run()
{
	while (STATE != GAMESTATE::EXIT)
	{
		ProcessInput();
	}
}

void ProcessInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_QUIT:
			STATE = GAMESTATE::EXIT;
			break;
		case SDL_KEYDOWN:
			std::cout << event.key.keysym.sym << std::endl;
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				STATE = GAMESTATE::EXIT;
				break;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				std::cout << "LEFT BUTTON" << std::endl;
				break;
			case SDL_BUTTON_RIGHT:
				std::cout << "RIGHT BUTTTON" << std::endl;
				break;
			case SDL_BUTTON_MIDDLE:
				std::cout << "MIDDLE BUTTON" << std::endl;
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			//std::cout << event.motion.x << " " << event.motion.y << std::endl;
			break;
		}
	}
}