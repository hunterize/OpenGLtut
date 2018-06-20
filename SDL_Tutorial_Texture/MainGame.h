#pragma once

#include <sdl\SDL.h>
#include <glew\glew.h>

#include "Sprite.h"
#include "GLSLProgram.h"
#include "GLTexture.h"

#include <vector>

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:
	void initSystems();
	void initShaders();
	void gameLoop();
	void processInput();
	void drawGame();
	void calculateFPS();

	SDL_Window* m_pWindow;
	int m_iScreenWidth;
	int m_iScreenHeight;
	
	GameState m_eState;
	//Sprite m_cSprite;


	/////vector.push_back call distruction function to delete the buffer
	std::vector<Sprite*> m_cSprites;

	GLSLProgram m_cColorProgram;
	//GLTexture m_cPlayerTexture;

	float m_fTime;
	float m_fFrameTime;
	float m_fFPS;
	float m_fMaxFPS;

};

