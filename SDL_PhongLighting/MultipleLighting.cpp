

#include "PhongLighting.h"

namespace MultipleLighting
{
	void ProcessInput();

	bool isRunning = true;
	int screenWidth = 800;
	int screenHeight = 600;
	float fov = 45.0f;

	CInputManager inputManager;

	bool isFirstMove = true;

	void GLSLMultipleLighting()
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

		window = SDL_CreateWindow("Phong Lighting Window - Multiple Lights", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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
		cubeShader.AttachShader("Shaders/LightingMultipleVertexShader.vert", "Shaders/LightingMultipleFragmentShader.frag");
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
			glm::vec3(15.0, 15.0, 7.0),
			glm::vec3(0, 15.0, 0),
			glm::vec3(-12.0, -12.0, -12.0),
			glm::vec3(-10.0, -7.0, 5.0),
			glm::vec3(10.0, 10.0, -5.0),
			glm::vec3(10.0, -6.0, 4.0),
			glm::vec3(-7.0, 8.0, 4.0),
			glm::vec3(-8.0, 6.0, -6.0),
			glm::vec3(12.0, 6.0, 8.0)

		};

		//light position
		glm::vec3 lightPos[] = {
			glm::vec3(20.0f, 0.0f, 0.0f),
			glm::vec3(18.0f, 18.0f, 18.0f),
			glm::vec3(-18.0f, -18.0f, 18.0f)
		};

		glm::vec3 lightCol[] = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)
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

				//update point lights position
				glm::mat4 lightRotation;

				//first point light
				glm::mat4 rot1 = glm::rotate(lightRotation, timespan * 0.6f, glm::vec3(0.0f, 1.0f, 0.0f));
				lightPos[0] = glm::vec3(rot1 * glm::vec4(lightPos[0], 1.0f));

				//second point light
				glm::mat4 rot2 = glm::rotate(lightRotation, timespan * 0.3f, glm::vec3(-1.0f, 0.0f, 1.0f));
				lightPos[1] = glm::vec3(rot2 * glm::vec4(lightPos[1], 1.0f));

				//third point light
				glm::mat4 rot3 = glm::rotate(lightRotation, timespan * 0.8f, glm::vec3(1.0f, 0.0f, 1.0f));
				lightPos[2] = glm::vec3(rot3 * glm::vec4(lightPos[2], 1.0f));

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
			cubeShader.SetUniformVec3("eyePos", eyePos);

			//set light color
			glm::vec3 directLightColor(1.0f, 1.0f, 1.0f);
			//glm::vec3 pointLightColor(1.0f, 1.0f, 1.0f);
			glm::vec3 spotLightColor(0.1f, 0.1f, 0.1f);

			float lightAmbientStrength = 0.1f;
			float lightDiffuseStrength = 0.75f;
			float lightSpecularStrength = 1.0f;

			glm::vec3 dirLightAmbient = 0.1f * lightAmbientStrength * directLightColor;
			glm::vec3 dirLightDiffuse = 0.2f * lightDiffuseStrength * directLightColor;
			glm::vec3 dirLightSpecular = 0.2f * lightSpecularStrength * directLightColor;

			glm::vec3 pointLightAmbient0 = lightAmbientStrength * lightCol[0];
			glm::vec3 pointLighDiffuse0 = lightDiffuseStrength * lightCol[0];
			glm::vec3 pointLightSpecular0 = lightSpecularStrength * lightCol[0];

			glm::vec3 pointLightAmbient1 = lightAmbientStrength * lightCol[1];
			glm::vec3 pointLighDiffuse1 = lightDiffuseStrength * lightCol[1];
			glm::vec3 pointLightSpecular1 = lightSpecularStrength * lightCol[1];

			glm::vec3 pointLightAmbient2 = lightAmbientStrength * lightCol[2];
			glm::vec3 pointLighDiffuse2 = lightDiffuseStrength * lightCol[2];
			glm::vec3 pointLightSpecular2 = lightSpecularStrength * lightCol[2];

			glm::vec3 spotLightAmbient = lightAmbientStrength * spotLightColor;
			glm::vec3 spotLighDiffuse = lightDiffuseStrength * spotLightColor;
			glm::vec3 spotLightSpecular = lightSpecularStrength * spotLightColor;

			//for point lights
				//light 1
			cubeShader.SetUniformVec3("pointLight[0].position", lightPos[0]);
			cubeShader.SetUniformVec3("pointLight[0].ambient", pointLightAmbient0);
			cubeShader.SetUniformVec3("pointLight[0].diffuse", pointLighDiffuse0);
			cubeShader.SetUniformVec3("pointLight[0].specular", pointLightSpecular0);
			cubeShader.SetUniformFloat("pointLight[0].constant", 1.0f);
			cubeShader.SetUniformFloat("pointLight[0].linear", 0.022f);
			cubeShader.SetUniformFloat("pointLight[0].quadratic", 0.0017f);
				//light 2
			cubeShader.SetUniformVec3("pointLight[1].position", lightPos[1]);
			cubeShader.SetUniformVec3("pointLight[1].ambient", pointLightAmbient1);
			cubeShader.SetUniformVec3("pointLight[1].diffuse", pointLighDiffuse1);
			cubeShader.SetUniformVec3("pointLight[1].specular", pointLightSpecular1);
			cubeShader.SetUniformFloat("pointLight[1].constant", 1.0f);
			cubeShader.SetUniformFloat("pointLight[1].linear", 0.022f);
			cubeShader.SetUniformFloat("pointLight[1].quadratic", 0.0017f);
				//light 3
			cubeShader.SetUniformVec3("pointLight[2].position", lightPos[2]);
			cubeShader.SetUniformVec3("pointLight[2].ambient", pointLightAmbient2);
			cubeShader.SetUniformVec3("pointLight[2].diffuse", pointLighDiffuse2);
			cubeShader.SetUniformVec3("pointLight[2].specular", pointLightSpecular2);
			cubeShader.SetUniformFloat("pointLight[2].constant", 1.0f);
			cubeShader.SetUniformFloat("pointLight[2].linear", 0.022f);
			cubeShader.SetUniformFloat("pointLight[2].quadratic", 0.0017f);

			//for directional light
			cubeShader.SetUniformVec3("dirLight.ambient", dirLightAmbient);
			cubeShader.SetUniformVec3("dirLight.diffuse", dirLightDiffuse);
			cubeShader.SetUniformVec3("dirLight.specular", dirLightSpecular);
			cubeShader.SetUniformVec3("dirLight.direction", glm::vec3(0.0f, -1.0f, 0.0f));

			
			//for spot light
			cubeShader.SetUniformVec3("spotLight.direction", camera.GetFront());
			cubeShader.SetUniformVec3("spotLight.position", eyePos);
			cubeShader.SetUniformVec3("spotLight.ambient", spotLightAmbient);
			cubeShader.SetUniformVec3("spotLight.diffuse", spotLighDiffuse);
			cubeShader.SetUniformVec3("spotLight.specular", spotLightSpecular);
			cubeShader.SetUniformFloat("spotLight.constant", 1.0f);
			cubeShader.SetUniformFloat("spotLight.linear", 0.022f);
			cubeShader.SetUniformFloat("spotLight.quadratic", 0.0017f);
			cubeShader.SetUniformFloat("spotLight.innercutoff", glm::cos(glm::radians(12.5f)));
			cubeShader.SetUniformFloat("spotLight.outercutoff", glm::cos(glm::radians(17.5f)));
			

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
				model = glm::scale(model, glm::vec3(7, 7, 7));

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

			for (int i = 0; i < sizeof(lightPos) / sizeof(glm::vec3); i++)
			{
				glm::mat4 lightModel;
				lightModel = glm::translate(lightModel, lightPos[i]);
				lightModel = glm::scale(lightModel, glm::vec3(2, 2, 2));
				lightShader.SetUniformMat4("model", lightModel);
				lightShader.SetUniformVec3("lightColor", lightCol[i]);

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}

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
