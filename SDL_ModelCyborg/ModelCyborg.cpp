#include "app.h"

namespace Cyborg
{
	void ProcessInput();

	bool isRunning = true;
	int screenWidth = 1600;
	int screenHeight = 900;
	float fov = 45.0f;

	CInputManager inputManager;

	bool isFirstMove = true;

	void GLAPIENTRY MessageCallback(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		fprintf(stderr, "GL CALLBACK: %s, source = 0x%x, type = 0x%x, id = 0x%x, severity = 0x%x, message = %s\n",
			(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
			source, type, id, severity, message);
	}


	void Cyborg()
	{
		//initial SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;

		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		//initialize camera
		CCamera3D camera(screenWidth, screenHeight, false,
			glm::vec3(20.0f, 10.0f, 20.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f));

		//create projection matrix
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

		camera.SetSpeed(6.0f);

		window = SDL_CreateWindow("Cyborg", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
		context = SDL_GL_CreateContext(window);

		SDL_ShowCursor(0);

		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			std::cout << "Glew init error" << std::endl;
			exit(0);
		}

		//enable opengl debug message
#ifdef DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#endif // DEBUG

		CShader floorShader;
		floorShader.AttachShader("Shaders/FloorVertexShader.vert", "Shaders/FloorFragmentShader.frag");
		CShader modelShader;
		modelShader.AttachShader("Shaders/ModelVertexShader.vert", "Shaders/ModelFragmentShader.frag");

		GLTexture floorTexture = CSTexture::LoadImage("GameResources/textures/brickwall.jpg");
		GLTexture floorNormalTexture = CSTexture::LoadImage("GameResources/textures/brickwall_normal.jpg");

		CModel soldierModel("GameResources/nanosuit/nanosuit.obj");
		CModel cyborgModel("GameResources/cyborg/cyborg.obj");

		glm::vec3 lightPos = glm::vec3(100.0f, 100.0f, 100.0f);
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

		//set floor vao and vbo
		GLuint floorVBO = 0;
		GLuint floorVAO = 0;
		GLfloat floorVertices[] = {
			//position           //normal           //uv         //tangent          //bitangent
			-0.5f, 0.0f, 0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, //bottom left
			0.5f, 0.0f, 0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, //bottom right
			0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, //top right
			0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, //top right
			-0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,	0.0f, 1.0f,	 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, //top left
			-0.5f, 0.0f, 0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f  //bottom left
		};

		glGenBuffers(1, &floorVBO);
		glGenVertexArrays(1, &floorVAO);

		glBindVertexArray(floorVAO);
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 6));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 8));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 11));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		//end of setting floor vao and vbo

		//initial time tick
		Uint32 previous = SDL_GetTicks();
		Uint32 lag = 0;
		Uint32 MS_PER_FRAME = 8;

		int eff = 1;

		while (isRunning)
		{
			glClearColor(0.1, 0.1, 0.1, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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

				glm::mat4 lightRotation;
				lightRotation = glm::rotate(lightRotation, timespan * 0.3f, glm::vec3(0.0f, 1.0f, 0.0f));
				lightPos = glm::vec3(lightRotation * glm::vec4(lightPos, 1.0));

				lag -= MS_PER_FRAME;
			}

			//synchronize the update and render
			Uint32 step = lag % MS_PER_FRAME;
			camera.Update(inputManager, (float)step / 1000);

			if (inputManager.IskeyPressed(SDLK_1))
			{
				eff = 1;
			}
			if (inputManager.IskeyPressed(SDLK_2))
			{
				eff = 2;
			}
			if (inputManager.IskeyPressed(SDLK_0))
			{
				eff = 0;
			}
			//create view matrix
			glm::mat4 view = camera.GetCameraMatrix();
			glm::vec3 eyePos = camera.GetPosition();
			float shininess = 128.0f;

			glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer

			//crate and floor use same shader
			floorShader.Use();
			floorShader.SetUniformMat4("view", view);
			floorShader.SetUniformMat4("projection", projection);

			floorShader.SetUniformInt("floorTex", 10);
			floorShader.SetUniformInt("normalMap", 11);


			floorShader.SetUniformVec3("eyePos", eyePos);
			floorShader.SetUniformVec3("lightPos", lightPos);
			floorShader.SetUniformVec3("lightColor", lightColor);
			floorShader.SetUniformFloat("shininess", shininess);

			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, floorTexture.ID);
			glActiveTexture(GL_TEXTURE11);
			glBindTexture(GL_TEXTURE_2D, floorNormalTexture.ID);

			for (int i = -5; i < 5; i++)
			{
				for (int j = -5; j < 5; j++)
				{
					glm::vec3 floorPos = glm::vec3(10.0f * i, 0.0f, 10.0f * j);
					glm::mat4 floorModel;
					floorModel = glm::translate(floorModel, floorPos);
					floorModel = glm::scale(floorModel, glm::vec3(10.0f, 10.0f, 10.0f));
					floorShader.SetUniformMat4("model", floorModel);
					glBindVertexArray(floorVAO);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glBindVertexArray(0);
				}
			}

			floorShader.Unuse();

			//render soldier
			modelShader.Use();
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f));
			model = glm::scale(model, glm::vec3(1.0f));

			modelShader.SetUniformMat4("model", model);
			modelShader.SetUniformMat4("view", view);
			modelShader.SetUniformMat4("projection", projection);

			modelShader.SetUniformVec3("eyePos", eyePos);
			modelShader.SetUniformVec3("lightPos", lightPos);
			modelShader.SetUniformVec3("lightColor", lightColor);

			modelShader.SetUniformFloat("shininess", shininess);
			modelShader.SetUniformInt("effect", eff);

			soldierModel.Render(modelShader);

			glm::mat4 cybModel = glm::mat4(1.0f);
			cybModel = glm::translate(cybModel, glm::vec3(-15.0f, 0.0, 0.0));
			cybModel = glm::scale(cybModel, glm::vec3(4.0f));
			modelShader.SetUniformMat4("model", cybModel);

			cyborgModel.Render(modelShader);

			modelShader.Unuse();
			//end of rendering soldier


			SDL_GL_SwapWindow(window);

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
}