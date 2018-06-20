#include "MainGame.h"

#include <iostream>
#include <string>

using namespace std;

void fatalError(string errorString)
{
	cout << errorString << endl;
	cout << "Enter any key to quit...";
	int tmp;
	cin >> tmp;
	SDL_Quit();
	exit(1);
}

MainGame::MainGame()
{
	m_pWindow = nullptr;
	m_iScreenWidth = 1024;
	m_iScreenHeight = 768;
	m_eState = GameState::PLAY;
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



}

void MainGame::run()
{
	initSystems();

	m_cSprite.init(-1.0f, -1.0f, 1.0f, 1.0f);

	gameLoop();

}

void MainGame::gameLoop()
{
	while (m_eState != GameState::EXIT)
	{
		processInput();
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

	//glEnableClientState(GL_COLOR_ARRAY);

	//draw a triangle using the immediate mode
	//opengl is using Normalized Device Coordinates(NDC) which is the x, y and z coordinates of each vertex should be between 01.0 and 1.0
	//coordinates outside this range will not be visible

	//glBegin(GL_TRIANGLES);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//glVertex2f(-1, -1);
	//glVertex2f(0, -1);
	//glVertex2f(0, 0);
	//glEnd();

	//glClearColor(0.0f, 0.0f, 1.0f, 1.0f);


	m_cSprite.draw();

	SDL_GL_SwapWindow(m_pWindow);
}