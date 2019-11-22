#include "SDL_OpenGLLightingAdv.h"

namespace PointShadow
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

	void PointShadow()
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

		window = SDL_CreateWindow("PointShadow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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

		//set debug vao and vbo
		GLuint debugVAO;
		GLuint debugVBO;
		GLfloat debugVertices[] = {
			//position       //uv
			-1.0f, 1.0f,     0.0f, 1.0f,    //top left
			-1.0f, -1.0f,    0.0f, 0.0f,    //bottom left
			1.0f,  -1.0f,    1.0f, 0.0f,    //bottom right
			1.0f,  -1.0f,    1.0f, 0.0f,    //bottom right
			1.0f,  1.0f,     1.0f, 1.0f,    //top right
			-1.0f, 1.0f,     0.0f, 1.0f    //top left
		};

		glGenBuffers(1, &debugVBO);
		glGenVertexArrays(1, &debugVAO);

		glBindVertexArray(debugVAO);
		glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(debugVertices), debugVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *)(sizeof(GLfloat) * 2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		//end of setting debug vao and vbo

		//frame buffer object for point shadow
		int depthWidth = 1024;
		int depthHeight = 1024;
		GLuint depthCubeMapFBO = 0;
		glGenFramebuffers(1, &depthCubeMapFBO);

		GLuint depthCubeMapTexture;
		glGenTextures(1, &depthCubeMapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapTexture);
		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, depthWidth, depthHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMapTexture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//check if the frame buffer with attachment is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Frame buffer is not complete!" << std::endl;
			exit(0);
		}
		//end of frame buffer object for point shadow

		CShader objShader;
		objShader.AttachShader("Shaders/PointShadowCrateVertexShader.vert", "Shaders/PointShadowCrateFragmentShader.frag");

		CShader shadowShader;
		shadowShader.AttachShader("Shaders/PointShadowDepthVertexShader.vert", "Shaders/PointShadowDepthFragmentShader.frag", "Shaders/PointShadowDepthGeometryShader.geo");

		CShader lightShader;
		lightShader.AttachShader("Shaders/PointShadowLightVertexShader.vert", "Shaders/PointShadowLightFragmentShader.frag");

		GLTexture woodTexture = CSTexture::LoadImage("wood.png");
		GLTexture crateTexture = CSTexture::LoadImage("crate.png");
		//create projection matrix
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

		glm::vec3 lightPos = glm::vec3(-10.0f, 8.0f, -10.0f);

		glm::vec3 pointLightPos = glm::vec3(-10.0f, 8.0f, -10.0f);
		float plNearPlane = 1.0f;
		float plFarPlane = 100.0f;
		glm::mat4 pointLightProjection = glm::perspective(glm::radians(90.0f), (float)depthWidth / depthHeight, plNearPlane, plFarPlane);
		std::vector<glm::mat4> plViews;

		//initial time tick
		Uint32 previous = SDL_GetTicks();
		Uint32 lag = 0;
		Uint32 MS_PER_FRAME = 8;

		while (isRunning)
		{
			glClearColor(0.1, 0.1, 0.1, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

			//glm::vec3 floorPos = glm::vec3(0.0f, -5.0f, 0.0f);
			glm::vec3 floorPos = glm::vec3(0.0f, 20.0f, 0.0f);
			glm::mat4 floorModel;
			floorModel = glm::translate(floorModel, floorPos);

			floorModel = glm::scale(floorModel, glm::vec3(80.0f, 50.0f, 80.0f));

			glm::vec3 cratePos[] = {
				glm::vec3(0.0f, 5.0f, 0.0f),
				glm::vec3(4.0f, -2.0f, 10.0f),
				glm::vec3(-5.0f, -3.499f, 0.0f),
				glm::vec3(-16.0f, 5.0f, -16.0f),
				glm::vec3(-25.0f, 20.0f, -10.0f),
				glm::vec3(-20.0f, 25.0f, -25.0f),
				glm::vec3(-4.0f, 15.0f, -4.0f)
			};

			int crateNum = sizeof(cratePos) / sizeof(glm::vec3);

			pointLightPos = glm::vec3(-10.0f, 10.0f, -10.0f);

			//for point light views
			plViews.push_back(
				pointLightProjection * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			plViews.push_back(
				pointLightProjection * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			plViews.push_back(
				pointLightProjection * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
			plViews.push_back(
				pointLightProjection * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
			plViews.push_back(
				pointLightProjection * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			plViews.push_back(
				pointLightProjection * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			//end for point light views

			//render light view to depth texture
			shadowShader.Use();
			glViewport(0, 0, depthWidth, depthHeight);

			glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
			glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
			glClear(GL_DEPTH_BUFFER_BIT);

			//draw floor 
			shadowShader.SetUniformFloat("far_plane", plFarPlane);
			shadowShader.SetUniformVec3("lightPos", pointLightPos);

			for (int i = 0; i < plViews.size(); i++)
			{
				shadowShader.SetUniformMat4("cubeMatrices[" + std::to_string(i) + "]", plViews[i]);
			}

			shadowShader.SetUniformMat4("model", floorModel);
			glBindVertexArray(crateVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			//draw crate			
			for (int i = 0; i < crateNum; i++)
			{
				glm::mat4 crateModel;
				crateModel = glm::translate(crateModel, cratePos[i]);
				crateModel = glm::scale(crateModel, glm::vec3(3.0f, 3.0f, 3.0f));
				shadowShader.SetUniformMat4("model", crateModel);

				glBindVertexArray(crateVAO);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}

			shadowShader.Unuse();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);;

			//game scene rendering
			glViewport(0, 0, screenWidth, screenHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.1, 0.1, 0.1, 1.0);

			//render light object
			lightShader.Use();
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
				
			objShader.Use();
			objShader.SetUniformMat4("projection", projection);
			objShader.SetUniformMat4("view", view);

			glm::vec3 eyePos = camera.GetPosition();
			objShader.SetUniformVec3("eyePos", eyePos);
			objShader.SetUniformVec3("lightPos", pointLightPos);
			objShader.SetUniformFloat("shininess", 64.0f);
			objShader.SetUniformFloat("far_plane", plFarPlane);
			objShader.SetUniformInt("isDebug", isDebug);
			objShader.SetUniformInt("isNormalReverse", true);

			//render floor
			objShader.SetUniformInt("sample", 20);
			glActiveTexture(GL_TEXTURE20);
			glBindTexture(GL_TEXTURE_2D, woodTexture.ID);

			objShader.SetUniformInt("shadowMap", 21);
			glActiveTexture(GL_TEXTURE21);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapTexture);

			objShader.SetUniformMat4("model", floorModel);
			glBindVertexArray(crateVAO);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			//render crates
			objShader.SetUniformInt("isNormalReverse", false);
			objShader.SetUniformInt("sample", 20);
			glActiveTexture(GL_TEXTURE20);
			glBindTexture(GL_TEXTURE_2D, crateTexture.ID);

			objShader.SetUniformInt("shadowMap", 21);
			glActiveTexture(GL_TEXTURE21);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapTexture);

			for (int i = 0; i < crateNum; i++)
			{
				glm::mat4 crateModel;
				crateModel = glm::translate(crateModel, cratePos[i]);
				crateModel = glm::scale(crateModel, glm::vec3(3.0f, 3.0f, 3.0f));
				objShader.SetUniformMat4("model", crateModel);

				glBindVertexArray(crateVAO);
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