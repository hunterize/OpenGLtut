#include "SDL_OpenGLLightingAdv.h"

namespace DeferredShading
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

	struct CLight
	{
		glm::vec3 m_Position;
		glm::vec3 m_color;
		GLfloat m_radius;
	};

	void DeferredShading()
	{
		//initial SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;

		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 4);

		//initialize camera
		CCamera3D camera(screenWidth, screenHeight, false,
			glm::vec3(50.0f, 50.0f, 50.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f));

		camera.SetSpeed(10.0f);

		window = SDL_CreateWindow("DeferredShading", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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

		//initialize vertices for game object
		GLuint cubeVBO = 0;
		GLuint cubeVAO = 0;
		GLuint cubeEBO = 0;

		//the cube, counterclock wise vertices
		GLfloat crateVertices[] = {
			//Pos                 //normal				//tangent           //bitangent         //texture coordinates
			//front side
			-0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,		//bottom left
			0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,		//bottom right
			0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,		//top right
			-0.5f, 0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,		//top left			
			//back side
			0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0,		-1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,	0.0f, 0.0f,		//bottom left
			-0.5f, -0.5f,  -0.5f,  0.0f, 0.0f, -1.0,	-1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,	1.0f, 0.0f,		//bottom right
			-0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, -1.0,		-1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,	1.0f, 1.0f,		//top right
			0.5f, 0.5f,  -0.5f,   0.0f, 0.0f, -1.0,		-1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,	0.0f, 1.0f,		//top left			
			//left side
			-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	0.0f, 0.0f,		//bottom left
			-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	1.0f, 0.0f,		//bottom right
			-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	1.0f, 1.0f,		//top right
			-0.5f, 0.5f,  -0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	0.0f, 1.0f,		//top left			
			//right side
			0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,	0.0f, 0.0f,		//bottom left
			0.5f, -0.5f,  -0.5f,  1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,	1.0f, 0.0f,		//bottom right
			0.5f,  0.5f,  -0.5f,  1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,	1.0f, 1.0f,		//top right
			0.5f, 0.5f,   0.5f,   1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,	0.0f, 1.0f,		//top left			
			//top side
			-0.5f, 0.5f,  0.5f,  0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,	0.0f, 0.0f,		//bottom left
			0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,	1.0f, 0.0f,		//bottom right
			0.5f,  0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,	1.0f, 1.0f,		//top right
			-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,	0.0f, 1.0f,		//top left			
			//bottom side
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,	1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,	0.0f, 0.0f,		//bottom left
			0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,	1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,	1.0f, 0.0f,		//bottom right
			0.5f,  -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,	1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,	1.0f, 1.0f,		//top right
			-0.5f,  -0.5f, 0.5f,  0.0f, -1.0f, 0.0f,	1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,	0.0f, 1.0f		//top left			
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
		glGenBuffers(1, &cubeVBO);
		glGenBuffers(1, &cubeEBO);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(crateVertices), crateVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(crateIndices), crateIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//set attribute location for vertex arrays
		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);

		//Attribute location = 0 in vertex shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)0);
		glEnableVertexAttribArray(0);
		//Attribute location = 1 in vertex shader
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);
		//Attribute location = 2 in vertex shader
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 6));
		glEnableVertexAttribArray(2);
		//Attribute location = 3 in vertex shader
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 9));
		glEnableVertexAttribArray(3);
		//Attribute location = 4 in vertex shader
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 12));
		glEnableVertexAttribArray(4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//end of crate vao

		//vertices for screen rendering 
		GLuint screenVAO = 0;
		GLuint screenVBO = 0;

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
		//end of screen vertices

		//config G-buffer
		GLuint gBufferFBO;
		GLuint gBufferRBO;
		glGenFramebuffers(1, &gBufferFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

		unsigned int gbPosition, gbNormal, gbAlbedo;

		//position g-buffer
		glGenTextures(1, &gbPosition);
		glBindTexture(GL_TEXTURE_2D, gbPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbPosition, 0);

		//normal g-buffer
		glGenTextures(1, &gbNormal);
		glBindTexture(GL_TEXTURE_2D, gbNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbNormal, 0);

		//albedo g-buffer, use RGBA format, alpha channel is for specular intensity
		glGenTextures(1, &gbAlbedo);
		glBindTexture(GL_TEXTURE_2D, gbAlbedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gbAlbedo, 0);

		unsigned int gbAttachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, gbAttachments);

		glGenRenderbuffers(1, &gBufferRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, gBufferRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBufferRBO);

		//check if the frame buffer with attachment is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Frame buffer is not complete!" << std::endl;
			exit(0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//end of configing g-buffer

		//config shaders
		CShader objShader;
		objShader.AttachShader("Shaders/DeferredGeometryVertexShader.vert", "Shaders/DeferredGeometryFragmentShader.frag");
		//CShader lightShader;
		//lightShader.AttachShader("Shaders/DeferredLightVertexShader.vert", "Shaders/DeferredLightFragmentShader.frag");
		CShader screenShader;
		screenShader.AttachShader("Shaders/DeferredScreenVertexShader.vert", "Shaders/DeferredScreenFragmentShader.frag");

		GLTexture crateTexture = CSTexture::LoadImage("brickwall.jpg");
		GLTexture crateNormal = CSTexture::LoadImage("brickwall_normal.jpg");

		//create projection matrix
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

		glm::vec3 pointLightPos = glm::vec3(30.0f, 20.0f, 30.0f);

		//1000 crates
		std::vector<glm::vec3> cratesPos;
		cratesPos.reserve(1000);

		for (int i = 0; i < 10; i++)
			for (int j = 0; j < 10; j++)
				for(int k = 0; k < 10; k++)
				{
					glm::vec3 pos = glm::vec3(150.0f - 30.0f * i, 150.0f - 30.0f * j, 150.0f - 30.0f * k);
					cratesPos.push_back(pos);
				}

		glm::vec3 obj[] = {
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 30.0f, 0.0f)
		};

		//1331 lights
		std::vector<CLight> cubeLights;
		cubeLights.reserve(1331);

		const GLfloat constant = 1.0f;
		const GLfloat linear = 0.7f;
		const GLfloat quadratic = 1.8f;

		for (int i = 0; i < 11; i++)
			for (int j = 0; j < 11; j++)
				for (int k = 0; k < 11; k++)
				{
					glm::vec3 pos = glm::vec3(165.0f - 30.0f * i, 165.0f - 30.0f * j, 165.0f - 30.0f * k);
					glm::vec3 color = glm::vec3(0.09f + 0.09 * i, 0.09f + 0.09 * j, 0.09f + 0.09 * k);
					const GLfloat brightness = std::fmaxf(std::fmaxf(color.x, color.y), color.z);
					GLfloat radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 0.1f) * brightness))) / (2.0f * quadratic);
					cubeLights.push_back({ pos, color, radius });
				}

		GLfloat exposure = 1.0f;
		bool isDebug = false;
		bool isBlur = false;

		//initial time tick
		Uint32 previous = SDL_GetTicks();
		Uint32 lag = 0;
		Uint32 MS_PER_FRAME = 8;

		while (isRunning)
		{
			glClearColor(0.1, 0.1, 0.1, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);  //enable depth testing

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

			glm::vec3 eyePos = camera.GetPosition();

			//bind g-buffer frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//render crates
			objShader.Use();
			objShader.SetUniformMat4("projection", projection);
			objShader.SetUniformMat4("view", view);

			objShader.SetUniformInt("sample", 20);
			glActiveTexture(GL_TEXTURE20);
			glBindTexture(GL_TEXTURE_2D, crateTexture.ID);

			objShader.SetUniformInt("nmap", 21);
			glActiveTexture(GL_TEXTURE21);
			glBindTexture(GL_TEXTURE_2D, crateNormal.ID);

			for (int i = 0; i < cratesPos.size(); i++)
			{
				glm::mat4 model;
				model = glm::translate(model, cratesPos[i]);
				model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));

				objShader.SetUniformMat4("model", model);
				glBindVertexArray(cubeVAO);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}

			objShader.Unuse();
			//end of rendering cubes
/*
			//render lights
			lightShader.Use();
			lightShader.SetUniformMat4("projection", projection);
			lightShader.SetUniformMat4("view", view);

			for (int i = 0; i < cubeLights.size(); i++)
			{
				glm::mat4 model;
				model = glm::translate(model, cubeLights[i].m_Position);
				model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
				lightShader.SetUniformMat4("model", model);
				lightShader.SetUniformVec3("lightColor", cubeLights[i].m_color);

				glBindVertexArray(cubeVAO);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
			lightShader.Unuse();
			//end of rendering lights
*/
//switch to default framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//render screen 		

			screenShader.Use();

			for (int i = 0; i < cubeLights.size(); i++)
			{
				screenShader.SetUniformVec3("lights[" + std::to_string(i) + "].position", cubeLights[i].m_Position);
				screenShader.SetUniformVec3("lights[" + std::to_string(i) + "].color", cubeLights[i].m_color);
				//std::cout << radius << std::endl;
				screenShader.SetUniformFloat("lights[" + std::to_string(i) + "].radius", cubeLights[i].m_radius);
			}

			screenShader.SetUniformInt("gPosition", 10);
			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, gbPosition);

			screenShader.SetUniformInt("gNormal", 11);
			glActiveTexture(GL_TEXTURE11);
			glBindTexture(GL_TEXTURE_2D, gbNormal);

			screenShader.SetUniformInt("gAlbedoSpecular", 12);
			glActiveTexture(GL_TEXTURE12);
			glBindTexture(GL_TEXTURE_2D, gbAlbedo);

			screenShader.SetUniformVec3("eyePos", eyePos);
			screenShader.SetUniformFloat("shininess", 128.0f);

			glBindVertexArray(screenVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);


			screenShader.Unuse();

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