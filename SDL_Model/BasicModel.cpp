//

#include "BasicModel.h"

#include <filesystem>


void ProcessInput();

bool isRunning = true;
int screenWidth = 800;
int screenHeight = 600;
float fov = 45.0f;

CInputManager inputManager;

bool isFirstMove = true;

void TestModule()
{

	//initial SDL
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;

	SDL_Init(SDL_INIT_EVERYTHING);

	//initialize camera
	CCamera3D camera(screenWidth, screenHeight,
		glm::vec3(0.0f, 5.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, -1.0f));

	window = SDL_CreateWindow("Test Modules", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

	context = SDL_GL_CreateContext(window);

	SDL_ShowCursor(0);

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "Glew init error" << std::endl;
		exit(0);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	glEnable(GL_DEPTH_TEST);

	//load model
	CModel soldierModel("GameResource/nanosuit/nanosuit.obj");

	//load shader
	CShader objShader;
	objShader.AttachShader("Shaders/ModelVertexShader.vert", "Shaders/ModelFragmentShader.frag");
	
	SDL_ShowWindow(window);
	


	//initial time tick
	Uint32 previous = SDL_GetTicks();
	Uint32 lag = 0;
	Uint32 MS_PER_FRAME = 8;

	while (isRunning)
	{
		//timer.Start();
		Uint32 current = SDL_GetTicks();

		Uint32 elapsed = current - previous;
		previous = current;
		lag += elapsed;

		float timespan = (float)MS_PER_FRAME / 1000;

		//update input
		ProcessInput();

		while (lag >= MS_PER_FRAME)
		{
			//update game
			//update camera
			camera.Update(inputManager, timespan);

			lag -= MS_PER_FRAME;
		}

		//synchronize the update and render
		Uint32 step = lag % MS_PER_FRAME;
		camera.Update(inputManager, (float)step / 1000);


		//render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		objShader.Use();

		//create projection matrix
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 1.0f, 1000.0f);
		objShader.SetUniformMat4("projection", projection);

		//create view matrix
		glm::mat4 view;
		view = camera.GetCameraMatrix();
		objShader.SetUniformMat4("view", view);

		//create model matrix
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		objShader.SetUniformMat4("model", model);

		soldierModel.Render(objShader);

		objShader.Unuse();


		SDL_GL_SwapWindow(window);
		//end of render

		//limit FPS to 60
		Uint32 last = SDL_GetTicks();
		Uint32 span = last - current;

		if (elapsed < 16)
		{
			SDL_Delay(16 - elapsed);
		}

	}

	SDL_GL_DeleteContext(context);
}


void ProcessInput()
{

	SDL_Event event;

	inputManager.Update();

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_SHOWN:
				SDL_WarpMouseInWindow(NULL, screenWidth / 2, screenHeight / 2);
				break;
			case SDL_WINDOWEVENT_LEAVE:
				isFirstMove = true;
				break;
			default:
				break;
			}

			break;
		case SDL_KEYDOWN:
			inputManager.PressKey(event.key.keysym.sym);
			break;

		case SDL_KEYUP:
			inputManager.Releasekey(event.key.keysym.sym);
			break;

		case SDL_MOUSEMOTION:
			if (isFirstMove)
			{
				SDL_WarpMouseInWindow(NULL, screenWidth / 2, screenHeight / 2);
				isFirstMove = false;
			}
			else
			{
				inputManager.SetMouseCoord(event.motion.x, event.motion.y);
				SDL_WarpMouseInWindow(NULL, screenWidth / 2, screenHeight / 2);
			}
			break;
		}
	}

	if (inputManager.IskeyPressed(SDLK_ESCAPE))
	{
		isRunning = false;
	}
}



int main(int args, char** argv)
{

	TestModule();

	return 1;
}
