#include "SDL_ModelSpace.h"


void ProcessInput();

bool isRunning = true;
int screenWidth = 1600/*1920*/;
int screenHeight = 900/*1080*/;
float fov = 45.0f;

CInputManager inputManager;

bool isFirstMove = true;
bool isDebug = false;

void GetDebugInfo(GLuint pID);

void AdvancedModel()
{
	//initial SDL
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow("Space", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);

	SDL_ShowCursor(0);

	//initialize glew
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "Glew init error" << std::endl;
		exit(0);
	}

	//initialize shader instance
	CShader planetShader;
	CShader asteroidShader;

	//load vertex shader and fragment shader
	planetShader.AttachShader("Shaders/PlanetVertexShaderSpace.vert", "Shaders/PlanetFragmentShaderSpace.frag");
	asteroidShader.AttachShader("Shaders/AsteroidVertexShaderSpace.vert", "Shaders/AsteroidFragmentShaderSpace.frag");


	//initialize soldier model
	//CModel soldierModel("GameResource/nanosuit/nanosuit.obj");
	CModel planetModel("GameResource/planet/planet.obj");
	glm::vec3 planetPos = glm::vec3(0.0f, 0.0f, 0.0f);

	CModel asteroidModel("GameResource/rock/rock.obj");
	float radius = 150.0f;
	glm::vec3 asteroidPos = glm::vec3(-radius, 0.0f, 0.0f);

	//light position
	glm::vec4 lightPos = glm::vec4(100.0f, 100.0f, 100.0f, 1.0f);
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 0.7f, 1.0f);
	float shininess = 32.0f;

	//initialize camera
	CCamera3D camera(screenWidth, screenHeight,
		glm::vec3(0.0f, 0.0f, 150.0f),
		glm::vec3(0.0f, 0.0f, -1.0f));
	camera.SetSpeed(10.5f);
	camera.SetSensitivity(2.0f);

	//setup uniform buffer object
	/*
	uniform buffer
	layout (std140) uniform uboBuffer
	{
		 mat4 projection;    0
		 mat4 view;          64
	}

	layout (std140) uniform uboLight
	{
		vec4 lightPos;       0
		vec4 lightColor;     16
		vec4 eyePos;         32
		float shininess;     48
	}

	*/
	GLuint ubo = 0;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	//glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBufferData(GL_UNIFORM_BUFFER, 128, NULL, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//end of setting ubo

	GLuint uboLight = 0;
	glGenBuffers(1, &uboLight);
	glBindBuffer(GL_UNIFORM_BUFFER, uboLight);
	glBufferData(GL_UNIFORM_BUFFER, 64, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//bind matrices uniform block to location index 0, lights uniform block to location index 1
	GLuint planetMatrixIndex = glGetUniformBlockIndex(planetShader.GetID(), "plaMatrices");
	std::cout << "planet matrix index: " << planetMatrixIndex << std::endl;
	glUniformBlockBinding(planetShader.GetID(), planetMatrixIndex, 0);
	GLuint planetLightIndex = glGetUniformBlockIndex(planetShader.GetID(), "plaLights");
	std::cout << "planet light index: " << planetLightIndex << std::endl;
	glUniformBlockBinding(planetShader.GetID(), planetLightIndex, 1);

	//bind matrices uniform block to location index 0, lights uniform block to location index 1
	GLuint asteroidUBOIndex = glGetUniformBlockIndex(asteroidShader.GetID(), "astMatrices");
	std::cout << "asteroid matrix index: " << asteroidUBOIndex << std::endl;
	glUniformBlockBinding(asteroidShader.GetID(), asteroidUBOIndex, 0);
	GLuint asteroidLightIndex = glGetUniformBlockIndex(asteroidShader.GetID(), "astLights");
	std::cout << "asteroid light index: " << asteroidLightIndex << std::endl;
	glUniformBlockBinding(asteroidShader.GetID(), asteroidLightIndex, 1);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 128);
	glBindBufferRange(GL_UNIFORM_BUFFER, 1, uboLight, 0, 64);

	//create projection matrix
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);
	//link projection matrix to ubo
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, uboLight);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, glm::value_ptr(lightPos));
	glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, glm::value_ptr(lightColor));
	glBufferSubData(GL_UNIFORM_BUFFER, 48, 16, &shininess);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//initial time tick
	Uint32 previous = SDL_GetTicks();
	Uint32 lag = 0;
	Uint32 MS_PER_FRAME = 8;

	while (isRunning)
	{
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

		//create view matrix
		glm::mat4 view = camera.GetCameraMatrix();
		glm::vec4 eyePos = glm::vec4(camera.GetPosition(), 1.0);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBuffer(GL_UNIFORM_BUFFER, uboLight);
		glBufferSubData(GL_UNIFORM_BUFFER, 32, 16, glm::value_ptr(eyePos));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		////render
		glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer

		
		//render planet
		planetShader.Use();

		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, planetPos);
		model = glm::scale(model, glm::vec3(5.0, 5.0, 5.0));
		planetShader.SetUniformMat4("model", model);

		if (inputManager.IskeyPressed(SDLK_1))
			planetShader.SetUniformInt("eff", 1);
		if (inputManager.IskeyPressed(SDLK_2))
			planetShader.SetUniformInt("eff", 2);

		planetModel.Render(planetShader);

		planetShader.Unuse();
		//end of rendering planet

		//render asteroid
		asteroidShader.Use();

		glm::mat4 asteroidMode = glm::mat4(1.0);
		asteroidMode = glm::translate(asteroidMode, asteroidPos);
		asteroidMode = glm::scale(asteroidMode, glm::vec3(1.0, 1.0, 1.0));

		asteroidShader.SetUniformMat4("model", asteroidMode);

		if (inputManager.IskeyPressed(SDLK_1))
			asteroidShader.SetUniformInt("eff", 1);
		if (inputManager.IskeyPressed(SDLK_2))
			asteroidShader.SetUniformInt("eff", 2);

		asteroidModel.Render(asteroidShader);

		asteroidShader.Unuse();
		//end of rendering asteroid

		SDL_GL_SwapWindow(window);

	} // end of main loop

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

void GetDebugInfo(GLuint pID)
{

}

int main(int args, char** argv)
{
	AdvancedModel();
	return 1;
}