#include "SDL_OpenGLLightingAdv.h"

namespace HDR
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
	};

	void HDR()
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
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, -1.0f));

		camera.SetSpeed(10.0f);

		window = SDL_CreateWindow("HDR", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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

		//initialize vertices for cube
		GLuint tunnelVBO = 0;
		GLuint tunnelVAO = 0;
		GLuint tunnelEBO = 0;

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
		glGenBuffers(1, &tunnelVBO);
		glGenBuffers(1, &tunnelEBO);

		glBindBuffer(GL_ARRAY_BUFFER, tunnelVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(crateVertices), crateVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tunnelEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(crateIndices), crateIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//set attribute location for vertex arrays
		glGenVertexArrays(1, &tunnelVAO);
		glBindVertexArray(tunnelVAO);
		glBindBuffer(GL_ARRAY_BUFFER, tunnelVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tunnelEBO);

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

		GLuint hdrVAO = 0;
		GLuint hdrVBO = 0;

		GLfloat hdrVertices[] = {
			//position          //uv
			-1.0f, 1.0f,        0.0f, 1.0f,  //top left
			-1.0f, -1.0f,       0.0f, 0.0f,  //bottom left
			1.0f,  -1.0f,       1.0f, 0.0f,  //bottom right
			1.0f,  -1.0f,       1.0f, 0.0f,  //bottom right
			1.0f,  1.0f,        1.0f, 1.0f,  //top right
			-1.0f, 1.0f,        0.0f, 1.0f   //top left
		};

		glGenBuffers(1, &hdrVBO);
		glGenVertexArrays(1, &hdrVAO);

		glBindVertexArray(hdrVAO);
		glBindBuffer(GL_ARRAY_BUFFER, hdrVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(hdrVertices), hdrVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		GLuint hdrFBO = 0;
		GLuint hdrTextureBuffer = 0;
		GLuint hdrRBO = 0;
		//set frame buffers
		//create frame buffer object
		glGenFramebuffers(1, &hdrFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

		//create color texture attachment
		glGenTextures(1, &hdrTextureBuffer);
		glBindTexture(GL_TEXTURE_2D, hdrTextureBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTextureBuffer, 0);

		//create render buffer attachment for depth and stencil 
		glGenRenderbuffers(1, &hdrRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, hdrRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, hdrRBO);

		//check if the frame buffer with attachment is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Frame buffer is not complete!" << std::endl;
			exit(0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//end of setting frame buffers

		CShader objShader;
		objShader.AttachShader("Shaders/HDRTunnelVertexShader.vert", "Shaders/HDRTunnelFragmentShader.frag");
		GLTexture tunnelTexture = CSTexture::LoadImage("wood.png", true);

		CShader hdrShader;
		hdrShader.AttachShader("Shaders/HDRScreenVertexShader.vert", "Shaders/HDRScreenFragmentShader.frag");

		//create projection matrix
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

		CLight lights[4];
		lights[0].m_Position = glm::vec3(0.0f, 0.0f, -75.0f);
		lights[0].m_color = glm::vec3(300.0f, 300.0f, 300.0f);
		lights[1].m_Position = glm::vec3(1.0f, 1.0f, -35.0f);
		lights[1].m_color = glm::vec3(1.1, 0.0, 0.0);
		lights[2].m_Position = glm::vec3(-1.0f, -1.0f, -55.0f);
		lights[2].m_color = glm::vec3(0.0, 0.6, 0.0);
		lights[3].m_Position = glm::vec3(1.0f, 0.0f, -25.0f);
		lights[3].m_color = glm::vec3(0.0, 0.0, 2.8);

		GLfloat exposure = 1.0f;
		bool isHDR = false;

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

			if (inputManager.IskeyPressed(SDLK_0))
			{
				isHDR = !isHDR;
			}

			//create view matrix
			glm::mat4 view = camera.GetCameraMatrix();

			glm::vec3 pointLightPos = glm::vec3(0.0f, 0.0f, -75.0f);

			glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
			glClearColor(0.1, 0.1, 0.1, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);  //enable depth testing
			
			//render floor
			objShader.Use();
			objShader.SetUniformMat4("projection", projection);
			objShader.SetUniformMat4("view", view);

			glm::vec3 eyePos = camera.GetPosition();
			objShader.SetUniformVec3("eyePos", eyePos);
			objShader.SetUniformVec3("lightPos", pointLightPos);
			objShader.SetUniformFloat("shininess", 4.0f);
			objShader.SetUniformInt("isNormalReverse", true);

			for (int i = 0; i < 4; i++)
			{
				objShader.SetUniformVec3("lights[" + std::to_string(i) + "].position", lights[i].m_Position);
				objShader.SetUniformVec3("lights[" + std::to_string(i) + "].color", lights[i].m_color);
			}

			glm::vec3 tunnelPos = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::mat4 tunnelModel;
			tunnelModel = glm::translate(tunnelModel, tunnelPos);
			tunnelModel = glm::scale(tunnelModel, glm::vec3(10.0f, 10.0f, 160.0f));

			objShader.SetUniformInt("sample", 20);
			glActiveTexture(GL_TEXTURE20);
			glBindTexture(GL_TEXTURE_2D, tunnelTexture.ID);

			objShader.SetUniformMat4("model", tunnelModel);
			glBindVertexArray(tunnelVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			objShader.Unuse();
			//end of rendering normal scene

			
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT);

			hdrShader.Use();
			hdrShader.SetUniformFloat("exposure", 1.0f);
			hdrShader.SetUniformInt("isHDR", isHDR);
			hdrShader.SetUniformInt("hdrSample", 1);
			glActiveTexture(GL_TEXTURE1);

			glBindVertexArray(hdrVAO);
			glBindTexture(GL_TEXTURE_2D, hdrTextureBuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			hdrShader.Unuse();
			
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