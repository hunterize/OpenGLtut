#include "MainGame.h"
#include "errors.h"
#include "ImageLoader.h"


#include <iostream>
#include <string>

using namespace std;

MainGame::MainGame()
{
	m_pWindow = nullptr;
	m_iScreenWidth = 1024;
	m_iScreenHeight = 768;
	m_eState = GameState::PLAY;
	m_fTime = 0.0f;
	m_cPlayerTexture = {};
}


MainGame::~MainGame()
{
}


void MainGame::initSystems()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	m_pWindow = SDL_CreateWindow(
		"Main Game", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		m_iScreenWidth, m_iScreenHeight, 
		SDL_WINDOW_OPENGL);

	if (m_pWindow == nullptr)
	{
		fatalError("SDL Window could not be created");
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(m_pWindow);
	if (glContext == nullptr)
	{
		fatalError("SDL_GL context could not be created");
	}

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		fatalError("Could not initialize GLEW");
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	initShaders();

}

void MainGame::initShaders()
{
	m_cColorProgram.CompileShaders("Shaders/ColorShading.vert", "Shaders/ColorShading.frag");
	m_cColorProgram.AddAttribute("vertexPosition");
	m_cColorProgram.AddAttribute("vertexColor");
	m_cColorProgram.AddAttribute("vertexUV");
	m_cColorProgram.LinkShaders();
}

void MainGame::run()
{
	initSystems();

	m_cPlayerTexture = ImageLoader::LoadPNG("Textures/PNG/CharacterRight_Standing.png");
	m_cSprite.init(-1.0f, -1.0f, 1.0f, 1.0f);

	gameLoop();

}

void MainGame::gameLoop()
{
	while (m_eState != GameState::EXIT)
	{
		processInput();
		m_fTime += 0.001f;
		drawGame();
	}
}

void MainGame::processInput()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			m_eState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			std::cout << event.motion.x << " " <<event.motion.y <<std::endl;
			break;

		case SDL_KEYDOWN:
			std::cout << event.key.keysym.sym << std::endl;
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				m_eState = GameState::EXIT;
				break;
			}
			break;
		}
	}
}

void MainGame::drawGame()
{
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_cColorProgram.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_cPlayerTexture.ID);

	GLuint textureLocation = m_cColorProgram.GetUniformLocation("mySampler");
	glUniform1i(textureLocation, 0);

	GLuint timeLocation = m_cColorProgram.GetUniformLocation("time");
	glUniform1f(timeLocation, m_fTime);

	m_cSprite.draw();


	glBindTexture(GL_TEXTURE_2D, 0);
	m_cColorProgram.unuse();

	SDL_GL_SwapWindow(m_pWindow);
}