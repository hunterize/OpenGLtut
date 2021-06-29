#include "SDL_ModelDeferred.h"

namespace ModelDeferred
{
	void ProcessInput();
	void InitializeScreenVertice();
	void InitializeCrateVertices();
	void InitializeFloorVertices();	
	void SetupGBuffer();

	//G buffer for geometry pass
	GLuint gBufferFBO = 0;
	GLuint gBufferRBO = 0;

	GLuint gbPosition = 0;
	GLuint gbNormal = 0;
	GLuint gbAlbedo = 0;

	//screen vertices for final lighting pass
	GLuint screenVAO = 0;
	GLuint screenVBO = 0;

	//crate vertices
	GLuint crateVBO = 0;
	GLuint crateVAO = 0;
	GLuint crateEBO = 0;

	//floor vertices
	GLuint floorVBO = 0;
	GLuint floorVAO = 0;

	CInputManager inputManager;

	//game initial data
	bool isRunning = true;
	int screenWidth = 1600;
	int screenHeight = 900;
	float fov = 45.0f;
	//initialize camera movement
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

	void ModelDeferred()
	{
		//initialize SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;
		
		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		window = SDL_CreateWindow("ModelDeferred", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
		context = SDL_GL_CreateContext(window);

		SDL_ShowCursor(0);

		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			std::cout << "Glew init error" << std::endl;
			exit(0);
		}

		//enable opengl debug message
#ifdef _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#endif // DEBUG

		//initialize camera
		CCamera3D camera(screenWidth, screenHeight, false, glm::vec3(20.0f, 30.0f, 20.0f), glm::vec3(-1.0f, -1.0f, -1.0f));
		camera.SetSpeed(6.0f);
		glm::mat4 view = camera.GetCameraMatrix();
		glm::vec3 eyePos = camera.GetPosition();

		//create project matrix
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

		//initializeVertices
		InitializeScreenVertice();
		InitializeCrateVertices();
		InitializeFloorVertices();

		//initialize G buffer
		SetupGBuffer();

		//load models
		CModel soldierModel("GameResources/nanosuit/nanosuit.obj");

		//load texture
		GLTexture floorTexture = CSTexture::LoadImage("GameResources/textures/wood.png");

		//load Shaders
		CShader modelShader;
		modelShader.AttachShader("Shaders/GeometryPassModel.vert", "Shaders/GeometryPassModel.frag");
		CShader gShader;
		gShader.AttachShader("Shaders/GeometryPassG.vert", "Shaders/GeometryPassG.frag");
		CShader lightingShader;
		lightingShader.AttachShader("Shaders/LightingPass.vert", "Shaders/LightingPass.frag");

		///initialize game objects
		//light
		glm::vec3 lightPos = glm::vec3(100.0f, 100.0f, 100.0f);
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		GLfloat linear = 0.01f;
		GLfloat quadratic = 0.003f;
		GLfloat shininess = 12.0f;
		//soldier
		glm::vec3 soldierPos = glm::vec3(0.0f, 0.0f, 0.0f);
		//floor
		glm::vec3 floorPos = glm::vec3(0.0f, -0.01f, 0.0f);
		//crate
		glm::vec3 cratePos = glm::vec3(10.0f, 2.0, 0.0f);

		//initlize time tick
		Uint32 previous = SDL_GetTicks();
		Uint32 lag = 0;
		Uint32 MS_PER_FRAME = 8;

		while (isRunning)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			Uint32 current = SDL_GetTicks();

			Uint32 elapsed = current - previous;
			previous = current;
			lag += elapsed;

			float timespan = (float)MS_PER_FRAME / 1000.0f;

			//update input
			ProcessInput();

			while (lag >= MS_PER_FRAME)
			{
				///update game				
				//update camera
				camera.Update(inputManager, timespan);
				lag -= MS_PER_FRAME;
			}

			//synchronize the update and render
			Uint32 step = lag % MS_PER_FRAME;
			camera.Update(inputManager, (float)step / 1000.0f);

			view = camera.GetCameraMatrix();
			eyePos = camera.GetPosition();

			//rendering
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			///geometry pass
			glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//render soldier
			modelShader.Use();
			modelShader.SetUniformMat4("view", view);
			modelShader.SetUniformMat4("projection", projection);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, soldierPos);
			model = glm::scale(model, glm::vec3(1.0f));
			modelShader.SetUniformMat4("model", model);
			soldierModel.Render(modelShader);
			modelShader.Unuse();

			//render floor
			gShader.Use();
			gShader.SetUniformMat4("view", view);
			gShader.SetUniformMat4("projection", projection);
			int isConverted = 0;
			gShader.SetUniformInt("isNormalInverted", isConverted);
			glm::mat4 floorModel = glm::mat4(1.0f);
			floorModel = glm::translate(floorModel, floorPos);
			floorModel = glm::scale(floorModel, glm::vec3(50.0f));
			gShader.SetUniformMat4("model", floorModel);
			gShader.SetUniformInt("hasTexture", 0);
			glBindVertexArray(floorVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			//render wall
			glm::mat4 wallModel = glm::mat4(1.0f);
			wallModel = glm::translate(wallModel, glm::vec3(0.0f, 10.0f, -25.0f));
			wallModel = glm::scale(wallModel, glm::vec3(50.0f, 20.0f, 50.0f));
			wallModel = glm::rotate(wallModel, (float)3.14 / 2.0f, glm::vec3(1.0, 0.0, 0.0));
			gShader.SetUniformMat4("model", wallModel);
			gShader.SetUniformInt("hasTexture", 0);
			glBindVertexArray(floorVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			//render crate, use same gshader
			glm::mat4 crateModel = glm::mat4(1.0f);
			crateModel = glm::translate(crateModel, cratePos);
			crateModel = glm::scale(crateModel, glm::vec3(4.0f));
			crateModel = glm::rotate(crateModel, (float)3.14 / 4.0f, glm::vec3(0.0, 1.0, 0.0));
			gShader.SetUniformMat4("model", crateModel);
			gShader.SetUniformInt("hasTexture", 1);
			gShader.SetUniformInt("sample", 20);
			glActiveTexture(GL_TEXTURE20);
			glBindTexture(GL_TEXTURE_2D, floorTexture.ID);
			glBindVertexArray(crateVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			gShader.Unuse();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			///lighting pass on default framebuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			lightingShader.Use();
			//light position in world space
			lightingShader.SetUniformVec3("lightPos", lightPos);
			lightingShader.SetUniformVec3("lightColor", lightColor);
			lightingShader.SetUniformVec3("eyePos", eyePos);
			lightingShader.SetUniformFloat("shininess", shininess);
			lightingShader.SetUniformFloat("linear", linear);
			lightingShader.SetUniformFloat("quadratic", quadratic);

			//bind geometry textures, set geometry textures uniform in lighting shader
			lightingShader.SetUniformInt("gPosition", 10);
			lightingShader.SetUniformInt("gNormal", 11);
			lightingShader.SetUniformInt("gAlbedo", 12);

			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, gbPosition);
			glActiveTexture(GL_TEXTURE11);
			glBindTexture(GL_TEXTURE_2D, gbNormal);
			glActiveTexture(GL_TEXTURE12);
			glBindTexture(GL_TEXTURE_2D, gbAlbedo);

			glBindVertexArray(screenVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			lightingShader.Unuse();
			
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

	void InitializeScreenVertice()
	{
		GLfloat screenVertices[] = {
			//position          //uv
			-1.0f, 1.0f,        0.0f, 1.0f,  //top left
			-1.0f, -1.0f,       0.0f, 0.0f,  //bottom left
			1.0f,  -1.0f,       1.0f, 0.0f,  //bottom right
			1.0f,  -1.0f,       1.0f, 0.0f,  //bottom right
			1.0f,  1.0f,        1.0f, 1.0f,  //top right
			-1.0f, 1.0f,        0.0f, 1.0f   //top left
		};

		glGenBuffers(1, &screenVBO);
		glGenVertexArrays(1, &screenVAO);

		glBindVertexArray(screenVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void InitializeCrateVertices()
	{

		//the cube, counterclock wise vertices
		GLfloat crateVertices[] = {
			//Pos                 //normal              //texture coordinates
			//front side
			-0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,		0.0f, 0.0f,	//bottom left
			0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 0.0f,	//bottom right
			0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 1.0f,	//top right
			-0.5f, 0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		0.0f, 1.0f,	//top left			
			//back side
			0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0,		0.0f, 0.0f,	//bottom left
			-0.5f, -0.5f,  -0.5f,  0.0f, 0.0f, -1.0,	1.0f, 0.0f,	//bottom right
			-0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, -1.0,		1.0f, 1.0f,	//top right
			0.5f, 0.5f,  -0.5f,   0.0f, 0.0f, -1.0,		0.0f, 1.0f,	//top left			
			//left side
			-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 0.0f,	//bottom left
			-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,	1.0f, 0.0f,	//bottom right
			-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,	1.0f, 1.0f,	//top right
			-0.5f, 0.5f,  -0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 1.0f,	//top left
			//right side
			0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,		0.0f, 0.0f,	//bottom left
			0.5f, -0.5f,  -0.5f,  1.0f, 0.0f, 0.0f,		1.0f, 0.0f,	//bottom right
			0.5f,  0.5f,  -0.5f,  1.0f, 0.0f, 0.0f,		1.0f, 1.0f,	//top right
			0.5f, 0.5f,   0.5f,   1.0f, 0.0f, 0.0f,		0.0f, 1.0f,	//top left			
			//top side
			-0.5f, 0.5f,  0.5f,  0.0f, 1.0f, 0.0f,		0.0f, 0.0f,	//bottom left
			0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,		1.0f, 0.0f,	//bottom right
			0.5f,  0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,		1.0f, 1.0f,	//top right
			-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,		0.0f, 1.0f,	//top left			
			//bottom side
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,	0.0f, 0.0f,	//bottom left
			0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,	1.0f, 0.0f,	//bottom right
			0.5f,  -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,	1.0f, 1.0f,	//top right
			-0.5f,  -0.5f, 0.5f,  0.0f, -1.0f, 0.0f,	0.0f, 1.0f	//top left			
		};

		//verex indices
		GLuint crateIndices[] = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4,
			8, 9, 10, 10, 11, 8,
			12, 13, 14, 14, 15, 12,
			16, 17, 18, 18, 19, 16,
			20, 21, 22, 22, 23, 20
		};

		//set vao for cube
		glGenBuffers(1, &crateVBO);
		glGenBuffers(1, &crateEBO);

		glBindBuffer(GL_ARRAY_BUFFER, crateVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(crateVertices), crateVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crateEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(crateIndices), crateIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//set attribute location for vertex arrays
		glGenVertexArrays(1, &crateVAO);
		glBindVertexArray(crateVAO);
		glBindBuffer(GL_ARRAY_BUFFER, crateVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crateEBO);

		//Attribute location = 0 in vertex shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)0);
		glEnableVertexAttribArray(0);
		//Attribute location = 1 in vertex shader
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);
		//Attribute location = 2 in vertex shader
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 6));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void InitializeFloorVertices()
	{
		GLfloat floorVertices[] = {
			//position           //normal           //uv
			-0.5f, 0.0f, 0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  //bottom left
			0.5f, 0.0f, 0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  //bottom right
			0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top right
			0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top right
			-0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,	0.0f, 1.0f,	//top left
			-0.5f, 0.0f, 0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f  //bottom left
		};

		glGenBuffers(1, &floorVBO);
		glGenVertexArrays(1, &floorVAO);

		glBindVertexArray(floorVAO);
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 6));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void SetupGBuffer()
	{
		glGenFramebuffers(1, &gBufferFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

		//position texture
		glGenTextures(1, &gbPosition);
		glBindTexture(GL_TEXTURE_2D, gbPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbPosition, 0);

		//normal texture
		glGenTextures(1, &gbNormal);
		glBindTexture(GL_TEXTURE_2D, gbNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbNormal, 0);

		//albedo texture, alpha channel is for specular intensity
		glGenTextures(1, &gbAlbedo);
		glBindTexture(GL_TEXTURE_2D, gbAlbedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gbAlbedo, 0);

		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		//set render buffer for depth and stencil
		glGenRenderbuffers(1, &gBufferRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, gBufferRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBufferRBO);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Frame buffer is not complete!" << std::endl;
			exit(0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}