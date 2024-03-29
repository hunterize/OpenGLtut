// SDL_OpenGLLighting.cpp : Defines the entry point for the application.
//

#include "SDL_OpenGLLighting.h"

void ProcessInput();

bool isRunning = true;
int screenWidth = 800;
int screenHeight = 600;
float fov = 45.0f;

CInputManager inputManager;

bool isFirstMove = true;

int main(int args, char** argv)
{

	//initial SDL
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;

	SDL_Init(SDL_INIT_EVERYTHING);

	//initialize shader instance
	CShader cubeShader;
	CShader lightShader;


	//initialize timer
	//CTimer timer(60);

	//initialize camera
	CCamera3D camera(screenWidth, screenHeight,
		glm::vec3(0.0f, 0.0f, 50.0f),
		glm::vec3(0.0f, 0.0f, -1.0f));

	window = SDL_CreateWindow("OpenGL Lighting Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "Glew init error" << std::endl;
		exit(0);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	glEnable(GL_DEPTH_TEST);

	//load vertex shader and fragment shader
	cubeShader.AttachShader("Shaders/BasicVertexShader.vert", "Shaders/BasicFragmentShader.frag");
	lightShader.AttachShader("Shaders/LightVertexShader.vert", "Shaders/LightFragmentShader.frag");

	//initialize vertices for cube
	GLuint vbo = 0;
	GLuint vao = 0;
	GLuint ebo = 0;

	//initialize vertices for light
	GLuint lightvao = 0;

	//the rectangle 
	GLfloat vertices[] = {
		//Pos               
		//front side
		-0.5f, -0.5f, 0.5f,    //bottom left
		-0.5f, 0.5f,  0.5f,   //top left
		0.5f,  0.5f,  0.5f,    //top right
		0.5f, -0.5f,  0.5f,     //bottom right
		//back side
		-0.5f, -0.5f, -0.5f,    //bottom left
		-0.5f, 0.5f,  -0.5f,    //top left
		0.5f,  0.5f,  -0.5f,    //top right
		0.5f, -0.5f,  -0.5f,     //bottom right
		//left side
		-0.5f, -0.5f, -0.5f,    //bottom left
		-0.5f, 0.5f,  -0.5f,    //top left
		-0.5f,  0.5f,  0.5f,   //top right
		-0.5f, -0.5f,  0.5f,     //bottom right
		//right side
		0.5f, -0.5f, -0.5f,   //bottom left
		0.5f, 0.5f,  -0.5f,    //top left
		0.5f,  0.5f,  0.5f,   //top right
		0.5f, -0.5f,  0.5f,     //bottom right
		//top side
		-0.5f, 0.5f,  -0.5f,   //bottom left
		0.5f,  0.5f,  -0.5f,    //top left
		0.5f,  0.5f,  0.5f,   //top right
		-0.5f,  0.5f,  0.5f,     //bottom right
		//bottom side
		-0.5f, -0.5f,  -0.5f,   //bottom left
		0.5f,  -0.5f,  -0.5f,   //top left
		0.5f,  -0.5f,  0.5f,    //top right
		-0.5f, -0.5f,  0.5f,     //bottom right
	};

	//verex indices
	GLuint indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

	//object positions
	glm::vec3 obj_pos[] = {
		glm::vec3(0.0, 0.0, 0.0),
		glm::vec3(8.0, 8.0, 5.0)
	};

	glm::vec3 lightPos = glm::vec3(15.0, 0.0, 0.0);


	//set vao for cube
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Attribute location = 0 in vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//end of setting vertex for cube

	//set vao for light
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &lightvao);

	glBindVertexArray(lightvao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Attribute location = 0 in vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//end of setting vertex for light

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
		//render cubes
		cubeShader.Use();

		//create projection matrix
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 1.0f, 1000.0f);
		cubeShader.SetUniformMat4("projection", projection);

		//create view matrix
		glm::mat4 view;
		//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		view = camera.GetCameraMatrix();
		cubeShader.SetUniformMat4("view", view);

		//set object color and light color
		glm::vec3 objColor(0.5f, 0.0f, 0.5f);
		glm::vec3 lightColor(1.0f, 1.0f, 0.0f);
		cubeShader.SetUniformVec3("fragmentColor", objColor);
		cubeShader.SetUniformVec3("lightColor", lightColor);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vao);

		//draw 

		for (int i = 0; i < sizeof(obj_pos) / sizeof(glm::vec3); i++)
		{
			//create mode matrix, model = TRS
			glm::mat4 model;
			model = glm::translate(model, obj_pos[i]);
			model = glm::rotate(model, i * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(6, 6, 6));

			cubeShader.SetUniformMat4("model", model);

			//draw the cube
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		glBindVertexArray(0);
		cubeShader.Unuse();
		//end of rendering cubes


		//render light
		lightShader.Use();

		glBindVertexArray(lightvao);
		lightShader.SetUniformMat4("projection", projection);
		lightShader.SetUniformMat4("view", view);
		glm::mat4 lightModel;
		lightModel = glm::translate(lightModel, lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(2, 2, 2));
		lightShader.SetUniformMat4("model", lightModel);

		lightShader.SetUniformVec3("lightColor", lightColor);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		lightShader.Unuse();

		//end of rendering light


		//float fps = timer.End();

		//std::cout << fps << std::endl;
		SDL_GL_SwapWindow(window);

		//limit FPS to 60
		Uint32 last = SDL_GetTicks();
		Uint32 span = last - current;

		if (elapsed < 16)
		{
			SDL_Delay(16 - elapsed);
		}
		//std::cout << "elapsed tick: " << elapsed << " frame tick: " << span << std::endl;

	}

	SDL_GL_DeleteContext(context);

	return 1;
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



