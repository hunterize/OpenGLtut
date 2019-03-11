

#include "PhongLighting.h"

namespace LightingMaps
{
	void ProcessInput();

	bool isRunning = true;
	int screenWidth = 800;
	int screenHeight = 600;
	float fov = 45.0f;

	CInputManager inputManager;

	bool isFirstMove = true;

	void GLSLLightingMaps()
	{
		//initial SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;

		SDL_Init(SDL_INIT_EVERYTHING);

		//initialize shader instance
		CShader cubeShader;
		CShader lightShader;


		//initialize timer
		//CTimer timer(60);

		//initialize camera
		CCamera3D camera(screenWidth, screenHeight,
			glm::vec3(0.0f, 0.0f, 50.0f),
			glm::vec3(0.0f, 0.0f, -1.0f));

		window = SDL_CreateWindow("Phong Lighting Window - Lighting Maps", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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

		//
		// these shaders are for lighting calculating in world space
		//
		//load vertex shader and fragment shader
		cubeShader.AttachShader("Shaders/LightingMapsVertexShader.vert", "Shaders/LightingMapsFragmentShader.frag");
		lightShader.AttachShader("Shaders/LightVertexShader.vert", "Shaders/LightFragmentShader.frag");


		//initialize textures
		GLTexture cubeTexture;
		GLTexture cubeSpecular;
		GLTexture cubeEmission;
		cubeTexture = CSTexture::LoadImage("crate.png");		
		cubeSpecular = CSTexture::LoadImage("Crate Specular.png");
		cubeEmission = CSTexture::LoadImage("crate emission.png");

		//initialize vertices for cube
		GLuint vbo = 0;
		GLuint vao = 0;
		GLuint ebo = 0;

		//initialize vertices for light
		GLuint lightvao = 0;

		//the rectangle 
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

		//object positions
		glm::vec3 obj_pos[] = {
			glm::vec3(0.0, 0.0, 0.0),
			//glm::vec3(25.0, 0.0, 0.0)
		};

		//light position
		glm::vec3 lightPos = glm::vec3(20.0, 0.0, 0.0);


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

		//set vao for light
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glGenVertexArrays(1, &lightvao);

		glBindVertexArray(lightvao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//Attribute location = 0 in vertex shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//end of setting vertex for light

		//initial time tick
		Uint32 previous = SDL_GetTicks();
		Uint32 lag = 0;
		Uint32 MS_PER_FRAME = 8;

		while (isRunning)
		{
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

				//update light position
				glm::mat4 lightRotation;
				lightRotation = glm::rotate(lightRotation, timespan * 0.6f, glm::vec3(0.0f, 1.0f, 0.0f));
				lightPos = glm::vec3(lightRotation * glm::vec4(lightPos, 1.0f));

				lag -= MS_PER_FRAME;
			}

			//synchronize the update and render
			Uint32 step = lag % MS_PER_FRAME;
			camera.Update(inputManager, (float)step / 1000);

			//render
			//render cubes
			cubeShader.Use();

			//create projection matrix
			glm::mat4 projection;
			projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 1.0f, 1000.0f);
			cubeShader.SetUniformMat4("projection", projection);

			//create view matrix
			glm::mat4 view;
			//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			view = camera.GetCameraMatrix();
			cubeShader.SetUniformMat4("view", view);

			//eyePos is only for the lighting calculation in world space
			glm::vec3 eyePos = camera.GetPosition();
			cubeShader.SetUniformVec3("eysPos", eyePos);

			//set light color
			glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
			float lightAmbientStrength = 0.2f;
			float lightDisffuseStrength = 0.75f;
			float lightSpecularStrength = 1.0f;
			glm::vec3 lightAmbient = lightAmbientStrength * lightColor;
			glm::vec3 lightDiffuse = lightDisffuseStrength * lightColor;
			glm::vec3 lightSpecular = lightSpecularStrength * lightColor;

			cubeShader.SetUniformVec3("light.position", lightPos);
			cubeShader.SetUniformVec3("light.ambient", lightAmbient);
			cubeShader.SetUniformVec3("light.diffuse", lightDiffuse);
			cubeShader.SetUniformVec3("light.specular", lightSpecular);

			//set object texture
			cubeShader.SetUniformInt("material.diffuse", 1);
			cubeShader.SetUniformInt("material.specular", 2);
			cubeShader.SetUniformInt("material.emission", 3);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, cubeTexture.ID);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, cubeSpecular.ID);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, cubeEmission.ID);

			float specularStrength = 32.0f;
			cubeShader.SetUniformFloat("material.shininess", specularStrength);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(vao);

			//draw 

			for (int i = 0; i < sizeof(obj_pos) / sizeof(glm::vec3); i++)
			{
				//create mode matrix, model = TRS
				glm::mat4 model;
				model = glm::translate(model, obj_pos[i]);
				model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				model = glm::scale(model, glm::vec3(15, 15, 15));

				cubeShader.SetUniformMat4("model", model);

				//draw the cube
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}

			glBindVertexArray(0);
			cubeShader.Unuse();
			//end of rendering cubes


			//render light
			lightShader.Use();

			glBindVertexArray(lightvao);
			lightShader.SetUniformMat4("projection", projection);
			lightShader.SetUniformMat4("view", view);

			glm::mat4 lightModel;
			lightModel = glm::translate(lightModel, lightPos);
			lightModel = glm::scale(lightModel, glm::vec3(2, 2, 2));
			lightShader.SetUniformMat4("model", lightModel);
			lightShader.SetUniformVec3("lightColor", lightColor);

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			lightShader.Unuse();

			//end of rendering light


			//float fps = timer.End();

			//std::cout << fps << std::endl;
			SDL_GL_SwapWindow(window);

			//limit FPS to 60
			Uint32 last = SDL_GetTicks();
			Uint32 span = last - current;

			if (elapsed < 16)
			{
				SDL_Delay(16 - elapsed);
			}
			//std::cout << "elapsed tick: " << elapsed << " frame tick: " << span << std::endl;

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
