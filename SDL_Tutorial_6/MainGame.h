#pragma once

#include <sdl\SDL.h>
#include <glew\glew.h>

#include "Sprite.h"

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:
	void initSystems();
	void gameLoop();
	void processInput();
	void drawGame();

	SDL_Window* m_pWindow;
	int m_iScreenWidth;
	int m_iScreenHeight;
	
	GameState m_eState;
	Sprite m_cSprite;

};

