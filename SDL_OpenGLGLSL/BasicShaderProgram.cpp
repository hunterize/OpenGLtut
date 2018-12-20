#include "SDL_OpenGLGLSL.h"

namespace BasicShaderProgram
{
	void ProcessInput();

	bool isRunning = true;
	int screenWidth = 800;
	int screenHeight = 600;


	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 50.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	GLfloat fov = 45.0f;

	void BasicShaderProgram()
	{

		//initial SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;

		SDL_Init(SDL_INIT_EVERYTHING);

		//initialize shader instance
		CShader shader;

		window = SDL_CreateWindow("Basic GLSL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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
		shader.AttachShader("Shaders/BasicVertexShader.vert", "Shaders/BasicFragmentShader.frag");

		//initialize vertices
		GLuint vbo = 0;
		GLuint vao = 0;
		GLuint ebo = 0;

		//the rectangle 
		GLfloat vertices[] = {
			//Pos                //color            //uv
			//front side
			-0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
			-0.5f, 0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
			0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
			0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0,   //bottom right
			//back side
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
			-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
			0.5f,  0.5f,  -0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
			0.5f, -0.5f,  -0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0,   //bottom right
			//left side
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
			-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
			-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0,   //bottom right
			//right side
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
			0.5f, 0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
			0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
			0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0,   //bottom right
			//top side
			-0.5f, 0.5f,  -0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
			0.5f,  0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
			0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
			-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0,   //bottom right
			//bottom side
			-0.5f, -0.5f,  -0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
			0.5f,  -0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
			0.5f,  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
			-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0   //bottom right
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


		//set vao
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
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 6));
		//glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//end of set vertex

		CTimer timer(60);

		while (isRunning)
		{
			timer.Start();

			unsigned int timespan = timer.GetTimeSpan();
			//std::cout <<"time span: "<< timespan << std::endl;

			ProcessInput();

			//use shader
			shader.Use();

			//create projection matrix
			glm::mat4 projection;
			projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 1.0f, 1000.0f);
			shader.SetUniformMat4("projection", projection);

			//create view matrix
			glm::mat4 view;
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			shader.SetUniformMat4("view", view);


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

				shader.SetUniformMat4("model", model);

				//draw the cube
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}


			
			glBindVertexArray(0);

			//unuse shader
			shader.Unuse();

			//SDL_Delay(100);

			float fps = timer.End();

			//std::cout << fps << std::endl;

			SDL_GL_SwapWindow(window);
		}

		SDL_GL_DeleteContext(context);
	}

	void ProcessInput()
	{

		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					isRunning = false;
					break;
				}
				break;

			default:
				break;
			}
		}
	}

}