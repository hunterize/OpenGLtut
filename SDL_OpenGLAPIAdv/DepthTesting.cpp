

#include "AdvancedOpenGL.h"


namespace DepthTesting 
{
	void ProcessInput();

	bool isRunning = true;
	int screenWidth = 800;
	int screenHeight = 600;
	float fov = 45.0f;

	CInputManager inputManager;

	bool isFirstMove = true;

	void DepthTesting()
	{
		//initial SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;

		SDL_Init(SDL_INIT_EVERYTHING);

		//initialize shader instance
		CShader cubeShader;

		//initialize camera
		CCamera3D camera(screenWidth, screenHeight,
			glm::vec3(0.0f, 0.0f, 50.0f),
			glm::vec3(0.0f, 0.0f, -1.0f));

		window = SDL_CreateWindow("Adv OpenGL - DepthTesting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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
		glDepthFunc(GL_LESS);
		//glDepthFunc(GL_ALWAYS);


		//load vertex shader and fragment shader
		cubeShader.AttachShader("Shaders/DepthTestingVertexShader.vert", "Shaders/DepthTestingFragmentShader.frag");

		//initialize textures
		GLTexture cubeTexture;
		GLTexture floorTexture;
		cubeTexture = CSTexture::LoadImage("marble.jpg");
		floorTexture = CSTexture::LoadImage("metal.png");

		//initialize vertices for cube
		GLuint vbo = 0;
		GLuint vao = 0;
		GLuint ebo = 0;

		GLuint floorVBO = 0;
		GLuint floorVAO = 0;


		//the cube 
		GLfloat vertices[] = {
			//Pos                 //normal              //texture coordinates
			//front side
			-0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,		0.0f, 0.0f,	//bottom left
			-0.5f, 0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		0.0f, 1.0f,	//top left
			0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 1.0f,	//top right
			0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 0.0f,	//bottom right
			//back side
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0,		1.0f, 0.0f,	//bottom left
			-0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, -1.0,		1.0f, 1.0f,	//top left
			0.5f,  0.5f,  -0.5f,  0.0f, 0.0f, -1.0,		0.0f, 1.0f,	//top right
			0.5f, -0.5f,  -0.5f,  0.0f, 0.0f, -1.0,		0.0f, 0.0f,	//bottom right
			//left side
			-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 0.0f,	//bottom left
			-0.5f, 0.5f,  -0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 1.0f,	//top left
			-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,	1.0f, 1.0f,	//top right
			-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,	1.0f, 0.0f,	//bottom right
			//right side
			0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,		1.0f, 0.0f,	//bottom left
			0.5f, 0.5f,  -0.5f,   1.0f, 0.0f, 0.0f,		1.0f, 1.0f,	//top left
			0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,		0.0f, 1.0f,	//top right
			0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,		0.0f, 0.0f,	//bottom right
			//top side
			-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,		0.0f, 0.0f,	//bottom left
			0.5f,  0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,		0.0f, 1.0f,	//top left
			0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,		1.0f, 1.0f,	//top right
			-0.5f, 0.5f,  0.5f,   0.0f, 1.0f, 0.0f,		1.0f, 0.0f,	//bottom right
			//bottom side
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,	1.0f, 0.0f,	//bottom left
			0.5f,  -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,	1.0f, 1.0f,	//top left
			0.5f,  -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,	0.0f, 1.0f,	//top right
			-0.5f, -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,	0.0f, 0.0f	//bottom right
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

		GLfloat floorVertices[] = {
			//position           //normal           //uv
			-0.5f, 0.0f, 0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  //bottom left
			-0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  //top left
			0.5f, 0.0f, 0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  //bottom right
			-0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  //top left
			0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top right
			0.5f, 0.0f, 0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f   //bottom right
		};

		//object positions
		glm::vec3 obj_pos = glm::vec3(0.0, 0.0, 0.0);
		
		glm::vec3 floor_pos = glm::vec3(0.0f, -5.1f, 0.0f);

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

		//set vao for floor
		glGenBuffers(1, &floorVBO);
		glGenVertexArrays(1, &floorVAO);

		glBindVertexArray(floorVAO);
		glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
		//set atrributes in vertex shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 6));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		//end of setting vertex for floor

		//initial time tick
		Uint32 previous = SDL_GetTicks();
		Uint32 lag = 0;
		Uint32 MS_PER_FRAME = 8;

		while (isRunning)
		{
			glClearColor(0.1, 0.1, 0.1, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

			//set object texture
			cubeShader.SetUniformInt("texture1", 1);
			glActiveTexture(GL_TEXTURE1);

			//create projection matrix
			glm::mat4 projection;
			projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 1.0f, 100.0f);
			cubeShader.SetUniformMat4("projection", projection);

			//create view matrix
			glm::mat4 view;
			view = camera.GetCameraMatrix();
			cubeShader.SetUniformMat4("view", view);

			//draw 

			//create mode matrix, model = TRS
			glm::mat4 model;
			model = glm::translate(model, obj_pos);
			model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::scale(model, glm::vec3(10, 10, 10));

			cubeShader.SetUniformMat4("model", model);

			//draw the cube
			glBindTexture(GL_TEXTURE_2D, cubeTexture.ID);
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			//end of rendering cubes


			//draw floor, use the same shader/projection and view matrix/
			glBindVertexArray(floorVAO);
			glm::mat4 floorMode;
			floorMode = glm::translate(floorMode, floor_pos);
			floorMode = glm::scale(floorMode, glm::vec3(30, 30, 30));

			cubeShader.SetUniformMat4("model", floorMode);
			glBindTexture(GL_TEXTURE_2D, floorTexture.ID);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			//end of drawing floor

			cubeShader.Unuse();

			SDL_GL_SwapWindow(window);

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
}