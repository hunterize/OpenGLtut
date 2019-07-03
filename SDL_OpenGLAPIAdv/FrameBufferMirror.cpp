
#include "AdvancedOpenGL.h"

namespace FrameBufferMirror
{
	void ProcessInput();

	bool isRunning = true;
	int screenWidth = 1600;
	int screenHeight = 900;

	int mirrorWidth = screenWidth / 4;
	int mirrorHeight = screenHeight / 4;

	float fov = 45.0f;

	CInputManager inputManager;

	bool isFirstMove = true;

	void FrameBufferMirror()
	{
		//initial SDL
		SDL_Window* window = nullptr;
		SDL_GLContext context = nullptr;

		SDL_Init(SDL_INIT_EVERYTHING);

		//initialize shader instance
		CShader cubeShader;
		CShader screenShader;
		CShader mirrorShader;

		//initialize camera
		CCamera3D camera(screenWidth, screenHeight,
			glm::vec3(0.0f, 0.0f, 50.0f),
			glm::vec3(0.0f, 0.0f, -1.0f));

		camera.SetSpeed(6.0f);
		camera.SetSensitivity(3.0f);

		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		window = SDL_CreateWindow("Adv OpenGL - FrameBufferMirror", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
		context = SDL_GL_CreateContext(window);

		SDL_ShowCursor(0);

		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			std::cout << "Glew init error" << std::endl;
			exit(0);
		}

		//load vertex shader and fragment shader
		cubeShader.AttachShader("Shaders/FrameBufferVertexShader.vert", "Shaders/FrameBufferFragmentShader.frag");
		screenShader.AttachShader("Shaders/FrameBufferScreenVertexShader.vert", "Shaders/FrameBufferScreenFragmentShader.frag");
		mirrorShader.AttachShader("Shaders/FrameBufferMirrorVertexShader.vert", "Shaders/FrameBufferMirrorFragmentShader.frag");

		//initialize textures
		GLTexture cubeTexture;
		GLTexture floorTexture;

		cubeTexture = CSTexture::LoadImage("crate.png");
		floorTexture = CSTexture::LoadImage("metal.png");

		//initialize vertices for cube
		GLuint vbo = 0;
		GLuint vao = 0;
		GLuint ebo = 0;

		GLuint floorVBO = 0;
		GLuint floorVAO = 0;

		GLuint screenVAO = 0;
		GLuint screenVBO = 0;
		GLuint screenFBO = 0;
		GLuint screenTextureBuffer = 0;
		GLuint screenRBO = 0;

		GLuint mirrorVAO = 0;
		GLuint mirrorVBO = 0;
		GLuint mirrorFBO = 0;
		GLuint mirrorTextureBuffer = 0;
		GLuint mirrorRBO = 0;

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

		GLfloat floorVertices[] = {
			//position           //normal           //uv
			-0.5f, 0.0f, 0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  //bottom left
			0.5f, 0.0f, 0.5f,    0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  //bottom right
			0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top right
			0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top right
			-0.5f, 0.0f, -0.5f,  0.0f, 1.0f, 0.0f,	0.0f, 1.0f,	//top left
			-0.5f, 0.0f, 0.5f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f  //bottom left
		};


		//vertices for screen with normalized coordinates, to simply render with shader
		GLfloat screenVertices[] = {
			//position       //uv
			-1.0f, 1.0f,     0.0f, 1.0f,    //top left
			-1.0f, -1.0f,    0.0f, 0.0f,    //bottom left
			1.0f,  -1.0f,    1.0f, 0.0f,    //bottom right
			1.0f,  -1.0f,    1.0f, 0.0f,    //bottom right
			1.0f,  1.0f,     1.0f, 1.0f,    //top right
			-1.0f, 1.0f,     0.0f, 1.0f    //top left
		};

		//vertices for mirror
		GLfloat mirrorVertices[] = {
			//position       //uv
			-0.5f, 0.5f,     0.0f, 1.0f,    //top left
			-0.5f, -0.5f,    0.0f, 0.0f,    //bottom left
			0.5f,  -0.5f,    1.0f, 0.0f,    //bottom right
			0.5f,  -0.5f,    1.0f, 0.0f,    //bottom right
			0.5f,  0.5f,     1.0f, 1.0f,    //top right
			-0.5f, 0.5f,     0.0f, 1.0f    //top left
		};
		

		/*
		GLfloat mirrorVertices[] = {
			0.0f, 0.0f,    0.0f, 1.0f,    //top left
			0.0f, 1.0f,    0.0f, 0.0f,	  //bottom left
			1.0f, 1.0f,    1.0f, 0.0f,    //bottom right
			1.0f, 1.0f,    1.0f, 0.0f,    //bottom right
			1.0f, 0.0f,    1.0f, 1.0f,    //top right
			0.0f, 0.0f,    0.0f, 1.0f     //top left
		};
		*/

		//object positions
		glm::vec3 obj_pos[] = {
			glm::vec3(0.0, 0.0, 0.0),
			glm::vec3(15.0, 0.0, 15.0),
			glm::vec3(-15.0, 0.0, -15.0),
			glm::vec3(15.0, 0.0, 0.0),
			glm::vec3(35.0, 0.0, 25.0),
			glm::vec3(-35.0, 0.0, 15.0),
			glm::vec3(10.0, 0.0, -35.0)
		};


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

		//set vao for screen
		glGenBuffers(1, &screenVBO);
		glGenVertexArrays(1, &screenVAO);

		glBindVertexArray(screenVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *)(sizeof(GLfloat) * 2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		//end of setting vertex for screen

		//set vao for mirror
		glGenBuffers(1, &mirrorVBO);
		glGenVertexArrays(1, &mirrorVAO);

		glBindVertexArray(mirrorVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mirrorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mirrorVertices), mirrorVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void *)(sizeof(GLfloat) * 2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		//end of setting vertex for mirror

		///set frame buffer for screen
		//create frame buffer object
		glGenFramebuffers(1, &screenFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);

		//create color texture attachment
		glGenTextures(1, &screenTextureBuffer);
		glBindTexture(GL_TEXTURE_2D, screenTextureBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTextureBuffer, 0);

		//create render buffer attachment for depth and stencil 
		glGenRenderbuffers(1, &screenRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, screenRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screenRBO);

		//check if the frame buffer with attachment is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Frame buffer is not complete!" << std::endl;
			exit(0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		///end of setting frame buffer for screen


		///set frame buffer for mirror
		//create frame buffer object
		glGenFramebuffers(1, &mirrorFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, mirrorFBO);

		//create color texture attachment
		glGenTextures(1, &mirrorTextureBuffer);
		glBindTexture(GL_TEXTURE_2D, mirrorTextureBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTextureBuffer, 0);

		//create render buffer attachment for depth and stencil 
		glGenRenderbuffers(1, &mirrorRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, mirrorRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mirrorRBO);

		//check if the frame buffer with attachment is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Frame buffer is not complete!" << std::endl;
			exit(0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		///end of setting frame buffer for mirror


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

			////render

			//bind to screen framebuffer and render to the buffer
			glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
			glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer

			///render cubes
			//create projection matrix
			glm::mat4 projection;
			projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 1.0f, 1000.0f);

			//create view matrix
			glm::mat4 view;
			view = camera.GetCameraMatrix();

			//cube and floor are using same shader
			cubeShader.Use();
			cubeShader.SetUniformMat4("projection", projection);
			cubeShader.SetUniformMat4("view", view);

			//set object texture
			cubeShader.SetUniformInt("sample", 1);
			glActiveTexture(GL_TEXTURE1);

			//draw cubes
			//enable face culling before drawing cubes
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK); //by default
			glFrontFace(GL_CCW); //by default

			for (int i = 0; i < sizeof(obj_pos) / sizeof(glm::vec3); i++)
			{
				//create mode matrix, model = TRS
				glm::mat4 model;
				model = glm::translate(model, obj_pos[i]);
				model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				model = glm::scale(model, glm::vec3(10, 10, 10));

				cubeShader.SetUniformMat4("model", model);

				//draw the cube
				glBindTexture(GL_TEXTURE_2D, cubeTexture.ID);
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
			///end of rendering cubes

			//draw floor
			glDisable(GL_CULL_FACE);
			glBindVertexArray(floorVAO);
			glm::mat4 floorMode;
			floorMode = glm::translate(floorMode, floor_pos);
			floorMode = glm::scale(floorMode, glm::vec3(160, 160, 160));

			cubeShader.SetUniformMat4("model", floorMode);
			glBindTexture(GL_TEXTURE_2D, floorTexture.ID);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			//end of drawing floor
			cubeShader.Unuse();

			//end of rendering to screen frame buffer

			//render to mirror
			//bind to mirror framebuffer and render to the buffer
			glBindFramebuffer(GL_FRAMEBUFFER, mirrorFBO);
			glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer

			///render cubes
			//create projection matrix
			//glm::mat4 projection;
			projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 1.0f, 1000.0f);

			//create view matrix
			//glm::mat4 view;
			view = camera.GetReverseCameraMatrix();

			//cube and floor are using same shader
			cubeShader.Use();
			cubeShader.SetUniformMat4("projection", projection);
			cubeShader.SetUniformMat4("view", view);

			//set object texture
			cubeShader.SetUniformInt("sample", 1);
			glActiveTexture(GL_TEXTURE1);

			//draw cubes
			//enable face culling before drawing cubes
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK); //by default
			glFrontFace(GL_CCW); //by default

			for (int i = 0; i < sizeof(obj_pos) / sizeof(glm::vec3); i++)
			{
				//create mode matrix, model = TRS
				glm::mat4 model;
				model = glm::translate(model, obj_pos[i]);
				model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				model = glm::scale(model, glm::vec3(10, 10, 10));

				cubeShader.SetUniformMat4("model", model);

				//draw the cube
				glBindTexture(GL_TEXTURE_2D, cubeTexture.ID);
				glBindVertexArray(vao);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
			///end of rendering cubes

			//draw floor
			glDisable(GL_CULL_FACE);
			glBindVertexArray(floorVAO);
			//glm::mat4 floorMode;
			floorMode = glm::mat4(1.0);
			floorMode = glm::translate(floorMode, floor_pos);
			floorMode = glm::scale(floorMode, glm::vec3(160, 160, 160));

			cubeShader.SetUniformMat4("model", floorMode);
			glBindTexture(GL_TEXTURE_2D, floorTexture.ID);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			//end of drawing floor
			cubeShader.Unuse();

			//end of rendering to screen frame buffer
			//end of rendering to mirror


			/// use frame buffer texture to render final scene

			//bind back default frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT);

			//use screen shader
			screenShader.Use();
			screenShader.SetUniformInt("screenSample", 1);
			glActiveTexture(GL_TEXTURE1);

			glBindVertexArray(screenVAO);
			glBindTexture(GL_TEXTURE_2D, screenTextureBuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			screenShader.Unuse();

			//render mirror
			glm::mat4 mirrorView = glm::mat4(1.0);
			glm::mat4 mirrorProjection = glm::ortho(-screenWidth / 2.0, screenWidth / 2.0, -screenHeight / 2.0, screenHeight / 2.0);
			
			//glm::mat4 mirrorProjection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f);
			glm::mat4 mirrorModel = glm::mat4(1.0);
			
			mirrorModel = glm::translate(mirrorModel, glm::vec3(-(screenWidth - mirrorWidth) / 2.0, (screenHeight - mirrorHeight) / 2.0, 0.0));
			//mirrorModel = glm::translate(mirrorModel, glm::vec3(0.0));
			mirrorModel = glm::scale(mirrorModel, glm::vec3(mirrorWidth, mirrorHeight, 1));

			mirrorShader.Use();

			mirrorShader.SetUniformMat4("view", mirrorView);
			mirrorShader.SetUniformMat4("projection", mirrorProjection);
			mirrorShader.SetUniformMat4("model", mirrorModel);

			mirrorShader.SetUniformInt("mirrorSample", 1);
			glActiveTexture(GL_TEXTURE1);

			glBindVertexArray(mirrorVAO);
			glBindTexture(GL_TEXTURE_2D, mirrorTextureBuffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			mirrorShader.Unuse();

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