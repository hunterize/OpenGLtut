
#include "AdvancedOpenGL.h"

namespace AdvancedAPI
{
	void ProcessInput();

	bool isRunning = true;
	int screenWidth = 1600;
	int screenHeight = 900;
	float fov = 45.0f;

	CInputManager inputManager;

	bool isFirstMove = true;

	void AdvancedAPI()
	{
		//initial SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;

		SDL_Init(SDL_INIT_EVERYTHING);

		//initialize shader instance
		CShader cubeShader;
		CShader redShader, greenShader, blueShader;


		//initialize camera
		CCamera3D camera(screenWidth, screenHeight,
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, -1.0f));

		window = SDL_CreateWindow("Adv OpenGL - AdvancedAPI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
		context = SDL_GL_CreateContext(window);

		SDL_ShowCursor(0);

		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			std::cout << "Glew init error" << std::endl;
			exit(0);
		}

		//load vertex shader and fragment shader
		cubeShader.AttachShader("Shaders/AdvAPIObjectVertexShader.vert", "Shaders/AdvAPIObjectFragmentShader.frag");
		redShader.AttachShader("Shaders/AdvAPICrateVertexShader.vert", "Shaders/AdvAPIRedFragmentShader.frag");
		greenShader.AttachShader("Shaders/AdvAPICrateVertexShader.vert", "Shaders/AdvAPIGreenFragmentShader.frag");
		blueShader.AttachShader("Shaders/AdvAPICrateVertexShader.vert", "Shaders/AdvAPIBlueFragmentShader.frag");

		//initialize textures
		GLTexture cubeTexture, cubeInnerTexture;
		cubeTexture = CSTexture::LoadImage("crate.png");
		cubeInnerTexture = CSTexture::LoadImage("marble.jpg");

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


		//object positions
		glm::vec3 obj_pos[] = {
			glm::vec3(0.0, 0.0, -20.0),
			glm::vec3(15.0, 0.0, 5.0),
			glm::vec3(-15.0, 0.0, -25.0),
			glm::vec3(15.0, 0.0, -10.0),
			glm::vec3(35.0, 0.0, 15.0),
			glm::vec3(-35.0, 0.0, 5.0),
			glm::vec3(10.0, 0.0, -45.0)
		};


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

		//setup uniform buffer object
		GLuint ubo = 0;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));
		//end of setting ubo

		//bind uniform buffer object index for cubes
		GLuint objectUBOindex = glGetUniformBlockIndex(cubeShader.GetID(), "Matrices");
		glUniformBlockBinding(cubeShader.GetID(), objectUBOindex, 0);

		//bind uniform buffer object index for red crate
		GLuint redUBOindex = glGetUniformBlockIndex(redShader.GetID(), "Matrices");
		glUniformBlockBinding(redShader.GetID(), redUBOindex, 0);

		//bind uniform buffer object index for green crate
		GLuint greenUBOindex = glGetUniformBlockIndex(greenShader.GetID(), "Matrices");
		glUniformBlockBinding(greenShader.GetID(), greenUBOindex, 0);

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

			//enable face culling before drawing cubes
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK); //by default
			glFrontFace(GL_CCW); //by default

			glEnable(GL_PROGRAM_POINT_SIZE);

			//render cubes
			
			glDisable(GL_CULL_FACE);
			cubeShader.Use();

			cubeShader.SetUniformInt("outside", 10);
			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, cubeTexture.ID);

			cubeShader.SetUniformInt("inside", 11);
			glActiveTexture(GL_TEXTURE11);
			glBindTexture(GL_TEXTURE_2D, cubeInnerTexture.ID);

			for (int i = 0; i < sizeof(obj_pos) / sizeof(glm::vec3); i++)
			{
				//create mode matrix, model = TRS
				glm::mat4 model;
				model = glm::translate(model, obj_pos[i]);
				model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				model = glm::scale(model, glm::vec3(10, 10, 10));

				cubeShader.SetUniformMat4("model", model);
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
			cubeShader.Unuse();
			glEnable(GL_CULL_FACE);
			//end of rendering cubes
			
			//render red cubes
			redShader.Use();
			for (int i = 0; i < sizeof(obj_pos) / sizeof(glm::vec3); i++)
			{
				//create mode matrix, model = TRS
				glm::mat4 model;
				model = glm::translate(model, obj_pos[i] + glm::vec3(0.0f, 12.0f, 0.0f));
				model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				model = glm::scale(model, glm::vec3(10, 10, 10));

				redShader.SetUniformMat4("model", model);
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
			redShader.Unuse();
			//end of rendering red cubes

			
			//render green cubes
			greenShader.Use();
			for (int i = 0; i < sizeof(obj_pos) / sizeof(glm::vec3); i++)
			{
				//create mode matrix, model = TRS
				glm::mat4 model;
				model = glm::translate(model, obj_pos[i] + glm::vec3(0.0f, 24.0f, 0.0f));
				model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				model = glm::scale(model, glm::vec3(10, 10, 10));

				greenShader.SetUniformMat4("model", model);
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
			greenShader.Unuse();
			//end of rendering green cubes

			//render blue cubes
			blueShader.Use();
			for (int i = 0; i < sizeof(obj_pos) / sizeof(glm::vec3); i++)
			{
				//create mode matrix, model = TRS
				glm::mat4 model;
				model = glm::translate(model, obj_pos[i] + glm::vec3(0.0f, -12.0f, 0.0f));
				model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				model = glm::scale(model, glm::vec3(10, 10, 10));

				blueShader.SetUniformMat4("model", model);
				glBindVertexArray(vao);
				//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glDrawElements(GL_POINTS, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
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