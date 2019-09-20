
#include "AdvancedOpenGL.h"

namespace GeometryShader
{
	void ProcessInput();

	bool isRunning = true;
	int screenWidth = 1600;
	int screenHeight = 900;
	float fov = 45.0f;

	CInputManager inputManager;

	bool isFirstMove = true;

	void GeometryShader()
	{
		//initial SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;

		SDL_Init(SDL_INIT_EVERYTHING);

		//initialize camera
		CCamera3D camera(screenWidth, screenHeight,
			glm::vec3(0.0f, 0.0f, 30.0f),
			glm::vec3(0.0f, 0.0f, -1.0f));

		window = SDL_CreateWindow("Adv OpenGL - GeometryShader", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
		context = SDL_GL_CreateContext(window);

		SDL_ShowCursor(0);

		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			std::cout << "Glew init error" << std::endl;
			exit(0);
		}

		//initialize shader instance
		CShader blueShader;

		//load vertex shader and fragment shader
		blueShader.AttachShader("Shaders/AdvAPIGeometryVertexShader.vert", "Shaders/AdvAPIGeometryFragmentShader.frag", "Shaders/AdvAPIGeometryShader.geom");

		//initialize vertices for cube
		GLuint vbo = 0;
		GLuint vao = 0;
		GLuint ebo = 0;

		//the cube, counterclock wise vertices
		GLfloat vertices[] = {
			//Pos                 //color             
			-0.5f,  -0.5f, -0.5f,  1.0f, 0.0f, 0.0,	//bottom left
			-0.5f,  0.5f,  -0.5f,  0.0f, 1.0f, 0.0,	//top left
			0.5f,   0.5f,  -0.5f,  0.0f, 0.0f, 1.0,	//top right
			0.5f,   -0.5f, -0.5f,  0.0f, 1.0f, 1.0  	//bottom right					
		};

		glm::vec3 obj_pos = glm::vec3(0.0, 0.0, 0.0);

		//set vao for cube
		glGenBuffers(1, &vbo);
		glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		//Attribute location = 0 in vertex shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void*)0);
		glEnableVertexAttribArray(0);
		//Attribute location = 1 in vertex shader
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void*)(sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//end of setting vertex for cube

		//setup uniform buffer object
		GLuint ubo = 0;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));
		//end of setting ubo
		
		//bind uniform buffer object index for blue crate
		GLuint blueUBOindex = glGetUniformBlockIndex(blueShader.GetID(), "Matrices");
		glUniformBlockBinding(blueShader.GetID(), blueUBOindex, 0);

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


			//create view matrix
			glm::mat4 view;
			view = camera.GetCameraMatrix();
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			////render
			glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer

			//glEnable(GL_PROGRAM_POINT_SIZE);

			//render blue cubes
			blueShader.Use();


			//create mode matrix, model = TRS
			glm::mat4 model;
			model = glm::translate(model, obj_pos);
			model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));

			blueShader.SetUniformMat4("model", model);

			glBindVertexArray(vao);
			glDrawArrays(GL_POINTS, 0, 4);
			glBindVertexArray(0);

			blueShader.Unuse();
			//end of rendering blue cubes

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
}