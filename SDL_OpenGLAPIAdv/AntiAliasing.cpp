
#include "AdvancedOpenGL.h"

namespace AntiAliasing
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

	void AntiAliasing()
	{
		//initial SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;

		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		//initialize camera
		CCamera3D camera(screenWidth, screenHeight, false,
			glm::vec3(20.0f, 0.0f, 20.0f),
			glm::vec3(-1.0f, 0.0f, -1.0f));

		camera.SetSpeed(6.0f);

		window = SDL_CreateWindow("AntiAliasing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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

		//it's enabled by default
		//glEnable(GL_MULTISAMPLE);

		//initialize shader instance
		CShader crateShader;
		CShader screenShader;

		//load vertex shader and fragment shader
		crateShader.AttachShader("Shaders/AntiAliasingVertexShader.vert", "Shaders/AntiAliasingFragmentShader.frag");
		screenShader.AttachShader("Shaders/AAScreenVertexShader.vert", "Shaders/AAScreenFragmentShader.frag");

		//initialize texture
		GLTexture crateTexture;
		GLTexture floorTexture;
		
		crateTexture = CSTexture::LoadImage("crate.png");
		floorTexture = CSTexture::LoadImage("metal.png");
		//initialize vertices for cube
		GLuint vbo = 0;
		GLuint vao = 0;
		GLuint ebo = 0;

		//the cube, counterclock wise vertices
		GLfloat vertices[] = {
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
		GLuint indices[] = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4,
			8, 9, 10, 10, 11, 8,
			12, 13, 14, 14, 15, 12,
			16, 17, 18, 18, 19, 16,
			20, 21, 22, 22, 23, 20
		};

		glm::vec3 objPos = glm::vec3(0.0, 0.0, 0.0);

		//set vao for cube
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//set attribute location for vertex arrays
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

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
		//end of setting vertex for cube

		//bind uniform buffer object index for blue crate
		GLuint crateIndex = 0;
		GLuint crateBindingPoint = glGetUniformBlockIndex(crateShader.GetID(), "Matrices");
		glUniformBlockBinding(crateShader.GetID(), crateBindingPoint, crateIndex);
		
		//setup uniform buffer object
		GLuint ubo = 0;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, crateIndex, ubo, 0, 2 * sizeof(glm::mat4));
		//end of setting ubo

		//set floor vao and vbo
		GLuint floorVBO = 0;
		GLuint floorVAO = 0;
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
		//end of setting floor vao and vbo

		glm::vec3 floorPos = glm::vec3(0.0f, -10.0f, 0.0f);

		//set screen vao and vbo
		GLuint screenVBO = 0;
		GLuint screenVAO = 0;
		GLfloat screenVertices[] = {
			//position       //uv
			-1.0f, 1.0f,     0.0f, 1.0f,    //top left
			-1.0f, -1.0f,    0.0f, 0.0f,    //bottom left
			1.0f,  -1.0f,    1.0f, 0.0f,    //bottom right
			1.0f,  -1.0f,    1.0f, 0.0f,    //bottom right
			1.0f,  1.0f,     1.0f, 1.0f,    //top right
			-1.0f, 1.0f,     0.0f, 1.0f    //top left
		};

		glGenBuffers(1, &screenVBO);
		glGenVertexArrays(1, &screenVAO);

		glBindVertexArray(screenVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *)(sizeof(GLfloat) * 2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		//end of setting screen vao and vbo

		//set frame buffer object for rendering
		GLuint screenFBO = 0;
		GLuint screenTextureBuffer = 0;
		GLuint screenRBO = 0;

		//set frame buffer object for MSAA
		GLuint screenFBOMSAA = 0;
		GLuint screenTextureBufferMSAA = 0;
		GLuint screenRBOMSAA = 0;


		glGenFramebuffers(1, &screenFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

		//create color texture attachment
		glGenTextures(1, &screenTextureBuffer);
		glBindTexture(GL_TEXTURE_2D, screenTextureBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, screenTextureBuffer);
		//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, screenWidth, screenHeight, GL_TRUE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTextureBuffer, 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, screenTextureBuffer, 0);

		//create render buffer attachment for depth and stencil
		glGenRenderbuffers(1, &screenRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, screenRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		//glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screenRBO);

		//check if the frame buffer with attachment is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Frame buffer is not complete!" << std::endl;
			exit(0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//end of setting frame buffer object


		//set framebuffer for MSAA
		glGenFramebuffers(1, &screenFBOMSAA);
		glBindFramebuffer(GL_FRAMEBUFFER, screenFBOMSAA);

		//create color texture attachment
		glGenTextures(1, &screenTextureBufferMSAA);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, screenTextureBufferMSAA);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, screenWidth, screenHeight, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, screenTextureBufferMSAA, 0);

		//create render buffer attachment for depth and stencil
		glGenRenderbuffers(1, &screenRBOMSAA);
		glBindRenderbuffer(GL_RENDERBUFFER, screenRBOMSAA);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screenRBOMSAA);

		//check if the frame buffer with attachment is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Frame buffer MSAA is not complete!" << std::endl;
			exit(0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//end of setting MSAA frame buffer object

		//create projection matrix
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);
		//link projection matrix to uniform buffer object
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//initial time tick
		Uint32 previous = SDL_GetTicks();
		Uint32 lag = 0;
		Uint32 MS_PER_FRAME = 8;

		int effect = 0;

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

			if (inputManager.IskeyPressed(SDLK_1))
				effect = 1;
			if (inputManager.IskeyPressed(SDLK_2))
				effect = 2;
			if (inputManager.IskeyPressed(SDLK_3))
				effect = 3;
			if (inputManager.IskeyPressed(SDLK_4))
				effect = 4;
			if (inputManager.IskeyPressed(SDLK_5))
				effect = 5;
			if (inputManager.IskeyPressed(SDLK_6))
				effect = 6;

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
			glm::mat4 view;
			view = camera.GetCameraMatrix();
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			////render
			//render to MSAA frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, screenFBOMSAA);

			glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer

			//render crate
			crateShader.Use();

			crateShader.SetUniformInt("outside", 10);
			glActiveTexture(GL_TEXTURE10);


			//create mode matrix, model = TRS
			glm::mat4 model;
			model = glm::translate(model, objPos);
			model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

			crateShader.SetUniformMat4("model", model);
			glBindTexture(GL_TEXTURE_2D, crateTexture.ID);

			//render floor, floor shares the shaders with the crate
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glm::mat4 floorModel;
			floorModel = glm::translate(floorModel, floorPos);
			floorModel = glm::scale(floorModel, glm::vec3(200.0f, 200.0f, 200.0f));
			crateShader.SetUniformMat4("model", floorModel);
			glBindTexture(GL_TEXTURE_2D, floorTexture.ID);

			glBindVertexArray(floorVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			crateShader.Unuse();
			//end of rendering objects

			glBindFramebuffer(GL_READ_FRAMEBUFFER, screenFBOMSAA);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screenFBO);
			glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			//bind default frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT);

			//render screen texture
			screenShader.Use();
			screenShader.SetUniformInt("eff", effect);
			screenShader.SetUniformInt("screenSample", 11);
			glActiveTexture(GL_TEXTURE11);

			glBindVertexArray(screenVAO);
			glBindTexture(GL_TEXTURE_2D, screenTextureBuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			screenShader.Unuse();

			////end of rendering

			SDL_GL_SwapWindow(window);

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
}