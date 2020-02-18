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

		//initialize vertices for cube
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

		CShader objShader;
		objShader.AttachShader("Shaders/DeferredObjectVertexShader.vert", "Shaders/DeferredObjectFragmentShader.frag");
		GLTexture crateTexture = CSTexture::LoadImage("brickwall.jpg");
		GLTexture crateNormal = CSTexture::LoadImage("brickwall_normal.jpg");

		//create projection matrix
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

		glm::vec3 pointLightPos = glm::vec3(30.0f, 20.0f, 30.0f);

		glm::vec3 obj[] = {
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 30.0f, 0.0f)
		};

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

				glm::mat4 lightRotation;
				lightRotation = glm::rotate(lightRotation, timespan * 0.3f, glm::vec3(0.0f, 1.0f, 0.0f));
				pointLightPos = glm::vec3(lightRotation * glm::vec4(pointLightPos, 1.0));

				lag -= MS_PER_FRAME;
			}

			//synchronize the update and render
			Uint32 step = lag % MS_PER_FRAME;
			camera.Update(inputManager, (float)step / 1000);


			//create view matrix
			glm::mat4 view = camera.GetCameraMatrix();

			//render crates
			objShader.Use();
			objShader.SetUniformMat4("projection", projection);
			objShader.SetUniformMat4("view", view);

			glm::vec3 eyePos = camera.GetPosition();
			objShader.SetUniformVec3("eyePos", eyePos);
			objShader.SetUniformVec3("lightPos", pointLightPos);
			objShader.SetUniformFloat("shininess", 128.0f);
			objShader.SetUniformInt("isNormalReverse", false);

			for (int i = 0; i < sizeof(obj) / sizeof(glm::vec3); i++)
			{
				glm::mat4 model;
				model = glm::translate(model, obj[i]);
				model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));

				objShader.SetUniformInt("sample", 20);
				glActiveTexture(GL_TEXTURE20);
				glBindTexture(GL_TEXTURE_2D, crateTexture.ID);

				objShader.SetUniformInt("nmap", 21);
				glActiveTexture(GL_TEXTURE21);
				glBindTexture(GL_TEXTURE_2D, crateNormal.ID);

				objShader.SetUniformMat4("model", model);
				glBindVertexArray(cubeVAO);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}

			objShader.Unuse();
			//end of rendering normal scene

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