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
	m_fFPS = 0.0f;
	m_fFrameTime = 0.0f;
	m_fMaxFPS = 60.0f;
	//m_cPlayerTexture = {};
}


MainGame::~MainGame()
{
}


void MainGame::initSystems()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	//set double buffer to avoid flicker
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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
	//check OpenGL version
	std::cout << "*** OpenGL Version is: " <<glGetString(GL_VERSION)<< std::endl;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//set VSYNC by 1. off by 0
	SDL_GL_SetSwapInterval(0);

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

	//m_cPlayerTexture = ImageLoader::LoadPNG("Textures/PNG/CharacterRight_Standing.png");
	//m_cSprite.init(-1.0f, -1.0f, 1.0f, 1.0f, "Textures/PNG/CharacterRight_Standing.png");

	Sprite* sprite1 = new Sprite();
	sprite1->init(-1.0f, -1.0f, 1.0f, 1.0f, "Textures/PNG/CharacterRight_Standing.png");

	m_cSprites.push_back(sprite1);

	Sprite* sprite2 = new Sprite();
	sprite2->init(-1.0f, 0.0f, 1.0f, 1.0f, "Textures/PNG/CharacterRight_Standing.png");

	m_cSprites.push_back(sprite2);



	gameLoop();

}

void MainGame::gameLoop()
{
	while (m_eState != GameState::EXIT)
	{
		float startTick = SDL_GetTicks();

		processInput();
		m_fTime += 0.001f;
		drawGame();
		calculateFPS();

		//print only noce every 10 frames
		static int frameCounter = 0;
		frameCounter++;
		if (frameCounter == 10)
		{
			std::cout << m_fFPS << endl;
			frameCounter = 0;
		}

		//limit the FPS to the max FPS
		float frameTicks = SDL_GetTicks() - startTick;

		//if (1000.0f / m_fMaxFPS > frameTicks)
		//{
		//	SDL_Delay(1000.0f / m_fMaxFPS - frameTicks);
		//}

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
			//std::cout << event.motion.x << " " <<event.motion.y <<std::endl;
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
	//glBindTexture(GL_TEXTURE_2D, m_cPlayerTexture.ID);

	GLuint textureLocation = m_cColorProgram.GetUniformLocation("mySampler");
	glUniform1i(textureLocation, 0);

	GLuint timeLocation = m_cColorProgram.GetUniformLocation("time");
	glUniform1f(timeLocation, m_fTime);

	//m_cSprite.draw();

	for each (Sprite* sprite in m_cSprites)
	{
		sprite->draw();
	}


	glBindTexture(GL_TEXTURE_2D, 0);
	m_cColorProgram.unuse();

	SDL_GL_SwapWindow(m_pWindow);
}


void MainGame::calculateFPS()
{
	static const int NUM_SAMPLES = 10;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;

	static float prevTicks = SDL_GetTicks();
	float currentTicks = SDL_GetTicks();

	m_fFrameTime = currentTicks - prevTicks;

	frameTimes[currentFrame % NUM_SAMPLES] = m_fFrameTime;

	//int ccc = (currentFrame < NUM_SAMPLES) ? currentFrame : NUM_SAMPLES;

	currentFrame++;

	int count;
	if (currentFrame < NUM_SAMPLES)
	{
		count = currentFrame;
	}
	else
	{
		count = NUM_SAMPLES;
	}




	float frameTimeAverage = 0;
	for (int i = 0; i < count; i++)
	{
		frameTimeAverage += frameTimes[i];
	}

	frameTimeAverage /= count;

	if (frameTimeAverage > 0)
	{
		m_fFPS = 1000.0f / frameTimeAverage;
	}
	else
	{
		m_fFPS = 60.0f;
	}

	prevTicks = currentTicks;



}