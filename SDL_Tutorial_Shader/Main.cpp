#include <iostream>



#include <SDL\SDL.h>
#include <GLEW\glew.h>

#include "MainGame.h"

int main(int argc, char** argv)
{
	MainGame mainGame;
	mainGame.run();

	return 0;
}