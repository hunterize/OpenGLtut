#include "SDL_OpenGLLightingAdv.h"

namespace NormalMapping
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

	GLfloat* GetTangentVertices(GLfloat* vArray, int size)
	{
		if (size != 192)
			return NULL;
		GLfloat* vertexArray = new GLfloat[336];

		int n = 0;
		for (int i = 0; i < size; i += 32 )
		{
			glm::vec3 p1 = glm::vec3(vArray[i], vArray[i + 1], vArray[i + 2]);
			glm::vec3 p2 = glm::vec3(vArray[i + 8], vArray[i + 9], vArray[i + 10]);
			glm::vec3 p3 = glm::vec3(vArray[i + 16], vArray[i + 17], vArray[i + 18]);

			glm::vec2 uv1 = glm::vec2(vArray[i + 6], vArray[i + 7]);
			glm::vec2 uv2 = glm::vec2(vArray[i + 14], vArray[i + 15]);
			glm::vec2 uv3 = glm::vec2(vArray[i + 22], vArray[i + 23]);

			GLfloat deltaU1 = uv1.x - uv2.x;
			GLfloat deltaU2 = uv3.x - uv2.x;
			GLfloat deltaV1 = uv1.y - uv2.y;
			GLfloat deltaV2 = uv3.y - uv2.y;

			glm::vec3 E1 = p1 - p2;
			glm::vec3 E2 = p3 - p2;

			GLfloat f = 1.0f / (deltaU1*deltaV2 - deltaV1 * deltaU2);

			glm::vec3 Tangent = f * (deltaV2 * E1 - deltaV1 * E2);
			glm::vec3 Bitangent = f * (deltaU1 * E2 - deltaU2 * E1);

			vertexArray[n + 8] = vertexArray[n + 8 + 14] = vertexArray[n + 8 + 28] = vertexArray[n + 8 + 42] = Tangent.x;
			vertexArray[n + 9] = vertexArray[n + 9 + 14] = vertexArray[n + 9 + 28] = vertexArray[n + 9 + 42] = Tangent.y;
			vertexArray[n + 10] = vertexArray[n + 10 + 14] = vertexArray[n + 10 + 28] = vertexArray[n + 10 + 42] = Tangent.z;

			vertexArray[n + 11] = vertexArray[n + 11 + 14] = vertexArray[n + 11 + 28] = vertexArray[n + 11 + 42] = Bitangent.x;
			vertexArray[n + 12] = vertexArray[n + 12 + 14] = vertexArray[n + 12 + 28] = vertexArray[n + 12 + 42] = Bitangent.y;
			vertexArray[n + 13] = vertexArray[n + 13 + 14] = vertexArray[n + 13 + 28] = vertexArray[n + 13 + 42] = Bitangent.z;

			for (int j = 0; j < 32; j++)
			{
				vertexArray[n] = vArray[i + j];
				if ((j + 1) % 8 == 0)
				{
					n += 7;
				}
				else
				{
					n++;
				}
			}
		}

		return vertexArray;
	}

	void NormalMapping()
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
			glm::vec3(30.0f, 20.0f, 30.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f));

		camera.SetSpeed(6.0f);

		window = SDL_CreateWindow("NormalMapping", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
		context = SDL_GL_CreateContext(window);

		SDL_ShowCursor(0);

		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			std::cout << "Glew init error" << std::endl;
			exit(0);
		}

		bool isDebug = false;
		bool isOrtho = true;

		//enable opengl debug message
#ifdef DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, 0);
#endif // DEBUG

		//initialize vertices for cube
		GLuint crateVBO = 0;
		GLuint crateVAO = 0;
		GLuint crateEBO = 0;

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

		GLfloat* tangentVer = GetTangentVertices(crateVertices, sizeof(crateVertices)/sizeof(GLfloat));

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
		glBufferData(GL_ARRAY_BUFFER, 336 * sizeof(GLfloat), tangentVer, GL_STATIC_DRAW);
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)0);
		glEnableVertexAttribArray(0);
		//Attribute location = 1 in vertex shader
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);
		//Attribute location = 2 in vertex shader
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 6));
		glEnableVertexAttribArray(2);
		//Attribute location = 3 in vertex shader
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 8));
		glEnableVertexAttribArray(3);
		//Attribute location = 4 in vertex shader
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 11));
		glEnableVertexAttribArray(4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete tangentVer;

		/*
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
		*/
		
		CShader objShader;
		objShader.AttachShader("Shaders/NormalMappingObjectVertexShader.vert", "Shaders/NormalMappingObjectFragmentShader.frag");

		CShader lightShader;
		lightShader.AttachShader("Shaders/NormalMappingLightVertexShader.vert", "Shaders/NormalMappingLightFragmentShader.frag");

		GLTexture wallTexture = CSTexture::LoadImage("brickwall.jpg");
		GLTexture normalTexture = CSTexture::LoadImage("brickwall_normal.jpg");

		//create projection matrix
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

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
				isDebug = !isDebug;
			}

			//create view matrix
			glm::mat4 view = camera.GetCameraMatrix();

			//render light object
			lightShader.Use();
			glm::vec3 pointLightPos = glm::vec3(-10.0f, 10.0f, -10.0f);
			glm::mat4 lightModel;
			lightModel = glm::translate(lightModel, pointLightPos);
			lightModel = glm::scale(lightModel, glm::vec3(1.0f));
			lightShader.SetUniformMat4("projection", projection);
			lightShader.SetUniformMat4("view", view);
			lightShader.SetUniformMat4("model", lightModel);
			glBindVertexArray(crateVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			lightShader.Unuse();
			//end of rendering light object

			//render floor
			objShader.Use();
			objShader.SetUniformMat4("projection", projection);
			objShader.SetUniformMat4("view", view);

			glm::vec3 eyePos = camera.GetPosition();
			objShader.SetUniformVec3("eyePos", eyePos);
			objShader.SetUniformVec3("lightPos", pointLightPos);
			objShader.SetUniformFloat("shininess", 256.0f);
			objShader.SetUniformInt("isDebug", isDebug);
			objShader.SetUniformInt("isNormalReverse", true);
			glm::vec3 wallPos = glm::vec3(0.0f, 20.0f, 0.0f);
			glm::mat4 wallModel;
			wallModel = glm::translate(wallModel, wallPos);
			wallModel = glm::scale(wallModel, glm::vec3(80.0f, 50.0f, 80.0f));
			objShader.SetUniformInt("sample", 20);
			glActiveTexture(GL_TEXTURE20);
			glBindTexture(GL_TEXTURE_2D, wallTexture.ID);
			objShader.SetUniformInt("nmap", 21);
			glActiveTexture(GL_TEXTURE21);
			glBindTexture(GL_TEXTURE_2D, normalTexture.ID);

			objShader.SetUniformMat4("model", wallModel);
			glBindVertexArray(crateVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

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