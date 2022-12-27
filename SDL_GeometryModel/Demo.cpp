#include "SDL_GeometryModel.h"

namespace Demo
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


	void Demo()
	{
		//initial SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;

		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		//initialize camera
		CCamera3D camera(screenWidth, screenHeight, false,
			glm::vec3(90.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f));

		//create projection matrix
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

		camera.SetSpeed(8.0f);

		window = SDL_CreateWindow("Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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

		//Geometry test

		CShader sphereShader;
		sphereShader.AttachShader("Shaders/SphereVertexShader.vert", "Shaders/SphereFragmentShader.frag");

		GLTexture albedoTexture = CSTexture::LoadImage("earth/albedo_4.jpg");
		GLTexture specularTexture = CSTexture::LoadImage("earth/earthspec1k.jpg");
		GLTexture bumpTexture = CSTexture::LoadImage("earth/height_4.jpg");

		//GLTexture albedoTexture = CSTexture::LoadImage("bricks2.jpg");
		//GLTexture specularTexture = CSTexture::LoadImage("bricks2_normal.jpg");
		//GLTexture bumpTexture = CSTexture::LoadImage("bricks2_disp.jpg");

		glm::vec3 spherePos = glm::vec3(0.0, 0.0, 0.0);
		glm::vec3 lightPos = glm::vec3(100.0);
		GLfloat radius = 60.0f;
		GLfloat sphereAngel = 0.0f;
		GLfloat rotateSpeed = 0.01f;

		GeometryFactory::GMesh sphereMesh;
		GeometryFactory::GGenerator::GetSphere(1000, 1000, sphereMesh);

		GLuint vaoSphere;
		GLuint vboSphere;
		GLuint eboSphere;

		//set vao for sphere
		glGenBuffers(1, &vboSphere);
		glGenBuffers(1, &eboSphere);

		glBindBuffer(GL_ARRAY_BUFFER, vboSphere);
		glBufferData(GL_ARRAY_BUFFER, sphereMesh.m_vertices.size() * sizeof(GeometryFactory::GVertex), &sphereMesh.m_vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboSphere);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereMesh.m_indices.size() * sizeof(GLuint), &sphereMesh.m_indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//set attribute location for vertex arrays
		glGenVertexArrays(1, &vaoSphere);
		glBindVertexArray(vaoSphere);
		glBindBuffer(GL_ARRAY_BUFFER, vboSphere);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboSphere);

		//Attribute location = 0 in vertex shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GeometryFactory::GVertex), (void*)0);
		glEnableVertexAttribArray(0);
		//Attribute location = 1 in vertex shader
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GeometryFactory::GVertex), (void*)offsetof(GeometryFactory::GVertex, m_normal));
		glEnableVertexAttribArray(1);
		//Attribute location = 2 in vertex shader
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GeometryFactory::GVertex), (void*)offsetof(GeometryFactory::GVertex, m_tangent));
		glEnableVertexAttribArray(2);
		//Attribute location = 3 in vertex shader
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(GeometryFactory::GVertex), (void*)offsetof(GeometryFactory::GVertex, m_texCoord));
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//end of setting vertex for sphere

		//End of Geometry test


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

			while (lag >= MS_PER_FRAME)
			{
				//update game
				//update camera
				camera.Update(inputManager, timespan);
				sphereAngel += rotateSpeed * timespan;

				lag -= MS_PER_FRAME;
			}

			//synchronize the update and render
			Uint32 step = lag % MS_PER_FRAME;
			camera.Update(inputManager, (float)step / 1000);
			sphereAngel += rotateSpeed * (float)step / 1000;

			//create view matrix
			glm::mat4 view = camera.GetCameraMatrix();

			glm::vec3 eyePos = camera.GetPosition();

			glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer

			sphereShader.Use();
			glm::mat4 sphereModel;
			sphereModel = glm::translate(sphereModel, spherePos);
			sphereModel = glm::scale(sphereModel, glm::vec3(2 * radius));
			sphereModel = glm::rotate(sphereModel, sphereAngel, glm::vec3(0.0f, 1.0f, 0.0f));
			sphereShader.SetUniformMat4("model", sphereModel);
			sphereShader.SetUniformMat4("view", view);
			sphereShader.SetUniformMat4("projection", projection);
			sphereShader.SetUniformVec3("eyePos", eyePos);
			sphereShader.SetUniformVec3("lightPos", lightPos);
			sphereShader.SetUniformFloat("shininess", 8.0);
			sphereShader.SetUniformFloat("heightScale", 0.005f);

			sphereShader.SetUniformInt("amap", 10);
			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, albedoTexture.ID);

			sphereShader.SetUniformInt("bmap", 11);
			glActiveTexture(GL_TEXTURE11);
			glBindTexture(GL_TEXTURE_2D, bumpTexture.ID);

			sphereShader.SetUniformInt("smap", 12);
			glActiveTexture(GL_TEXTURE12);
			glBindTexture(GL_TEXTURE_2D, specularTexture.ID);

			glBindVertexArray(vaoSphere);
			glDrawElements(GL_TRIANGLES, sphereMesh.m_indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			sphereShader.Unuse();


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