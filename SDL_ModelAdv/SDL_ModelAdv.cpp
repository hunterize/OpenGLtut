#include "SDL_ModelAdv.h"


void ProcessInput();

bool isRunning = true;
int screenWidth = 1600;
int screenHeight = 900;
float fov = 45.0f;

CInputManager inputManager;

bool isFirstMove = true;

void AdvancedModel()
{
	//initial SDL
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow("AdvModel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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
	CShader skyShader;
	CShader soldierShader;
	//load vertex shader and fragment shader
	skyShader.AttachShader("Shaders/SkyVertexShader.vert", "Shaders/SkyFragmentShader.frag");
	soldierShader.AttachShader("Shaders/ModelVertexShader.vert", "Shaders/ModelFragmentShader.frag");

	//initialize soldier model
	CModel soldierModel("GameResource/nanosuit/nanosuit.obj");

	//initialize textures
	GLTexture skyTexture;

	skyTexture.ID = CSTexture::LoadCubeMaps("GameResource/skybox/front.jpg",
		"GameResource/skybox/back.jpg",
		"GameResource/skybox/top.jpg",
		"GameResource/skybox/bottom.jpg",
		"GameResource/skybox/left.jpg",
		"GameResource/skybox/right.jpg");

	//light position
	glm::vec3 lightPos = glm::vec3(-100.0f, 100.0f, -100.0f);

	//initialize camera
	CCamera3D camera(screenWidth, screenHeight,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f));
	camera.SetSpeed(1.5f);
	camera.SetSensitivity(1.0f);

	//initialize vertices for cube

	GLuint skyVBO = 0;
	GLuint skyVAO = 0;

	GLfloat skyVertices[] = {
		//position		
		//right side x=1.0
		1.0f, 1.0f, -1.0f,             //top left
		1.0f, -1.0f, -1.0f,            //bottom left
		1.0f, -1.0f, 1.0f,             //bottom right
		1.0f, -1.0f, 1.0f,             //bottom right
		1.0f, 1.0f, 1.0f,              //top right
		1.0f, 1.0f, -1.0f,             //top left
		//top side y=1.0
		-1.0f, 1.0f, 1.0f,             //top left
		-1.0f, 1.0f, -1.0f,            //bottom left
		1.0f, 1.0f, -1.0f,             //bottom right
		1.0f, 1.0f, -1.0f,             //bottom right
		1.0f, 1.0f, 1.0f,              //top right
		-1.0f, 1.0f, 1.0f,              //top left
		//left side x=-1.0
		-1.0f, 1.0f, 1.0f,             //top left
		-1.0f, -1.0f, 1.0f,            //bottom left
		-1.0f, -1.0f, -1.0f,           //bottom right
		-1.0f, -1.0f, -1.0f,           //bottom right
		-1.0f, 1.0f,  -1.0f,           //top right
		-1.0f, 1.0f, 1.0f,             //top left
		//bottom side y=-1.0
		-1.0f, -1.0f, -1.0f,           //top left
		-1.0f, -1.0f, 1.0f,            //bottom left
		1.0f, -1.0f, 1.0f,             //bottom right
		1.0f, -1.0f, 1.0f,             //bottom right
		1.0f, -1.0f, -1.0f,            //top right
		-1.0f, -1.0f, -1.0f,           //top left
		//back side z=1.0
		1.0f, 1.0f, 1.0f,              //top left
		1.0f, -1.0f, 1.0f,             //bottom left
		-1.0f, -1.0f, 1.0f,            //bottom right
		-1.0f, -1.0f, 1.0f,            //bottom right
		-1.0f, 1.0f, 1.0f,             //top right
		1.0f, 1.0f, 1.0f,              //top left
		//front side  z=-1.0
		-1.0f, 1.0f,  -1.0f,            //top left
		-1.0f, -1.0f, -1.0f,            //bottom left
		1.0f,  -1.0f, -1.0f,            //bottom right
		1.0f,  -1.0f, -1.0f,            //bottom right
		1.0f,  1.0f,  -1.0f,            //top right
		-1.0f, 1.0f,  -1.0f            //top left
		//
	};

	//set vao for sky box
	glGenBuffers(1, &skyVBO);
	glGenVertexArrays(1, &skyVAO);

	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//end of setting vao for sky box

	//initial time tick
	Uint32 previous = SDL_GetTicks();
	Uint32 lag = 0;
	Uint32 MS_PER_FRAME = 8;

	while (isRunning)
	{
		glClearColor(0.1, 0.1, 0.1, 1.0);
		//glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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


		//create projection matrix
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

		//create view matrix
		glm::mat4 view;
		view = camera.GetCameraMatrix();


		////render
		glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer

		//draw sky box
		glDisable(GL_DEPTH_TEST);

		
		skyShader.Use();
		view = glm::mat4(glm::mat3(view));
		skyShader.SetUniformMat4("view", view);
		skyShader.SetUniformMat4("projection", projection);
		skyShader.SetUniformInt("skybox", 0);

		glBindVertexArray(skyVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture.ID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		skyShader.Unuse();

		//end of drawing sky box
		glEnable(GL_DEPTH_TEST);

		
		//render soldier
		view = camera.GetCameraMatrix();
		soldierShader.Use();

		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0, -10.0, -20.0));
		model = glm::scale(model, glm::vec3(1.0, 1.0, 1.0));

		soldierShader.SetUniformMat4("model", model);
		soldierShader.SetUniformMat4("view", view);
		soldierShader.SetUniformMat4("projection", projection);

		glm::vec3 eyePos = camera.GetPosition();
		soldierShader.SetUniformVec3("eyePos", eyePos);

		soldierShader.SetUniformVec3("lightPos", lightPos);

		float shininess = 32.0;
		soldierShader.SetUniformFloat("shininess", shininess);

		soldierModel.Render(soldierShader);

		soldierShader.Unuse();

		//end of rendering soldier

		////end of rendering

		SDL_GL_SwapWindow(window);

		//limit FPS to 60
		Uint32 last = SDL_GetTicks();
		Uint32 span = last - current;

		if (elapsed < 16)
		{
			SDL_Delay(16 - elapsed);
		}
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

int main(int args, char** argv)
{
	AdvancedModel();
	return 1;
}