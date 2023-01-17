#include "SDL_OpenGLProject.h"

namespace Demo
{
	void ProcessInput();

	const int DEFAULT_SCREENWIDTH = 1600;
	const int DEFAULT_SCREENHEIGHT = 900;

	bool isRunning = true;
	int screenWidth = DEFAULT_SCREENWIDTH;
	int screenHeight = DEFAULT_SCREENHEIGHT;
	float fov = 45.0f;

	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;
	bool isFullscreen = false;

	SDL_SysWMinfo wInfo;
	HMONITOR hMonitor = NULL;
	DEVICE_SCALE_FACTOR sFactor;

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


	void Demo()
	{
		//initial SDL
		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetSwapInterval(0);


		window = SDL_CreateWindow("Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

		context = SDL_GL_CreateContext(window);

		SDL_ShowCursor(0);

#ifdef DEBUG
		float ddpi, hdpi, vdpi;
		if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi) != 0)
		{
			std::cout << "Failed to get display dpi - " << SDL_GetError() << std::endl;
		}
		else
		{
			std::cout << "Display DPI - " << std::endl;
			std::cout << "ddpi: " << ddpi << std::endl;
			std::cout << "hdpi: " << hdpi << std::endl;
			std::cout << "vdpi: " << vdpi << std::endl;
		}		

		SDL_VERSION(&wInfo.version);
		if (SDL_GetWindowWMInfo(window, &wInfo))
		{
			hMonitor = MonitorFromWindow(wInfo.info.win.window, MONITOR_DEFAULTTONEAREST);
			std::cout << "Monitor Handle: " << hMonitor << std::endl;
			if (!GetScaleFactorForMonitor(hMonitor, &sFactor))
			{
				std::cout << "Monitor Scale Factor is: " << sFactor << std::endl;
			}
			else
			{
				std::cout << "GetScalFactorForMonitor failed: " << std::endl;
			}
		}
		else
		{
			std::cout << SDL_GetError() << std::endl;
		}
#endif
		
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


		//initialize camera
		CCamera3D camera(screenWidth, screenHeight, false,
			glm::vec3(20.0f, 10.0f, 20.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f));

		camera.SetSpeed(3.0f);


		CShader demoShader;
		demoShader.AttachShader("Shaders/DemoVertexShader.vert", "Shaders/DemoFragmentShader.frag");

		GLTexture woodTexture = CSTexture::LoadImage("wood.png");
		GLTexture crateTexture = CSTexture::LoadImage("crate.png");


		//initialize vertices for crate
		GLuint crateVBO = 0;
		GLuint crateVAO = 0;
		GLuint crateEBO = 0;

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

		glm::vec3 cratePos = glm::vec3(0.0, 0.0, 0.0);

		//set vao for cube
		glGenBuffers(1, &crateVBO);
		glGenBuffers(1, &crateEBO);

		glBindBuffer(GL_ARRAY_BUFFER, crateVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, crateEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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
		//end of setting vertex for crate


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

		glm::vec3 floorPos = glm::vec3(0.0f, -5.0f, 0.0f);

		//initial time tick
		Uint32 previous = SDL_GetTicks();
		Uint32 lag = 0;
		Uint32 MS_PER_FRAME = 8;

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

			//update camera
			camera.UpdateScreen(screenWidth, screenHeight);
			camera.Update(inputManager, (float) elapsed / 1000);

			
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

			//create projection matrix
			glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

			glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer


			//crate and floor use same shader
			demoShader.Use();
			glm::mat4 floorModel;
			floorModel = glm::translate(floorModel, floorPos);
			floorModel = glm::scale(floorModel, glm::vec3(30.0f, 30.0f, 30.0f));
			demoShader.SetUniformMat4("model", floorModel);
			demoShader.SetUniformMat4("view", view);
			demoShader.SetUniformMat4("projection", projection);
			glBindTexture(GL_TEXTURE_2D, woodTexture.ID);

			glBindVertexArray(floorVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);


			glm::mat4 crateModel;
			crateModel = glm::translate(crateModel, cratePos);
			crateModel = glm::scale(crateModel, glm::vec3(2.0f, 2.0f, 2.0f));
			demoShader.SetUniformMat4("model", crateModel);

			glBindTexture(GL_TEXTURE_2D, crateTexture.ID);

			glBindVertexArray(crateVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			demoShader.Unuse();


			SDL_GL_SwapWindow(window);
			
			/*
			if (elapsed < 16)
			{
				SDL_Delay(16 - elapsed);
			}
			*/
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
					std::cout << "window show event" << std::endl;
					if (isFullscreen)
					{
						SDL_WarpMouseGlobal(screenWidth / 2, screenHeight / 2);
					}
					else
					{
						SDL_WarpMouseInWindow(window, screenWidth / 2, screenHeight / 2);
					}
					break;
				case SDL_WINDOWEVENT_LEAVE:
					std::cout << "window exit event" << std::endl;
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
					if (isFullscreen)
					{
						SDL_WarpMouseGlobal(screenWidth / 2, screenHeight / 2);
					}
					else
					{
						SDL_WarpMouseInWindow(window, screenWidth / 2, screenHeight / 2);
					}
					isFirstMove = false;
				}
				else
				{
					inputManager.SetMouseCoord(event.motion.x, event.motion.y);
					
					if (isFullscreen)
					{
						SDL_WarpMouseGlobal(screenWidth / 2, screenHeight / 2);
					}
					else
					{
						SDL_WarpMouseInWindow(window, screenWidth / 2, screenHeight / 2);
					}
				}
				break;
			}
		}

		if (inputManager.IskeyPressed(SDLK_ESCAPE))
		{
			isRunning = false;
		}

		if (inputManager.IskeyPressed(SDLK_F2))
		{
			isFullscreen = !isFullscreen;

			if (isFullscreen)
			{
				SDL_DisplayMode dm;
#ifdef DEBUG
				int display_mode_count;
				std::cout << "Number of Video Displays: " << SDL_GetNumVideoDisplays() << std::endl;
				display_mode_count = SDL_GetNumDisplayModes(0);
				std::cout << "Number of Display Modes: " << display_mode_count << std::endl;
				for (int i = 0; i < display_mode_count; i++)
				{
					SDL_GetDisplayMode(0, i, &dm);
					std::cout << "Display Mode #" << i << std::endl;
					std::cout << "Width: " << dm.w << std::endl;
					std::cout << "Height: " << dm.h << std::endl;
					std::cout << "FreshRate: " << dm.refresh_rate << std::endl;
				}

#endif

				//set native fullscreen display mode
				SDL_GetDisplayMode(0, 0, &dm);
				SDL_SetWindowDisplayMode(window, &dm);
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

#ifdef DEBUG
				SDL_GetCurrentDisplayMode(0, &dm);
				std::cout << "Current Display Mode - " << std::endl;
				std::cout << "Width: " << dm.w << std::endl;
				std::cout << "Height: " << dm.h << std::endl;
				std::cout << "FreshRate: " << dm.refresh_rate << std::endl;				

				SDL_Rect rec;
				SDL_GetDisplayBounds(0, &rec);
				std::cout << "Display Bounds Rectangular - " << std::endl;
				std::cout << "Position.x: " << rec.x << std::endl;
				std::cout << "Position.y: " << rec.y << std::endl;
				std::cout << "Rec.Width: " << rec.w << std::endl;
				std::cout << "Rec.Height: " << rec.h << std::endl;
#endif

				//update screen width and height
				screenWidth = dm.w;
				screenHeight = dm.h;

				SDL_WarpMouseGlobal(screenWidth / 2, screenHeight / 2);
			}
			else
			{
				SDL_SetWindowFullscreen(window, NULL);
				screenWidth = DEFAULT_SCREENWIDTH;
				screenHeight = DEFAULT_SCREENHEIGHT;

				SDL_WarpMouseInWindow(window, screenWidth / 2, screenHeight / 2);
			}

			//isFirstMove = false;
			glViewport(0, 0, screenWidth, screenHeight);

		}
	}
}