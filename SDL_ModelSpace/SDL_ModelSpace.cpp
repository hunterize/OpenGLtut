#include "SDL_ModelSpace.h"


void ProcessInput();

bool isRunning = true;
int screenWidth = 1600/*1920*/;
int screenHeight = 900/*1080*/;
float fov = 45.0f;

CInputManager inputManager;

bool isFirstMove = true;
bool isDebug = false;

void GetDebugInfo(GLuint pID);

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

void AdvancedModel()
{
	//initial SDL
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow("Space", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);

	SDL_ShowCursor(0);

	//initialize glew
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

	//initialize shader instance
	CShader planetShader;
	CShader asteroidShader;

	//load vertex shader and fragment shader
	planetShader.AttachShader("Shaders/PlanetVertexShaderSpace.vert", "Shaders/PlanetFragmentShaderSpace.frag");
	asteroidShader.AttachShader("Shaders/AsteroidVertexShaderSpace.vert", "Shaders/AsteroidFragmentShaderSpace.frag");


	//initialize soldier model
	//CModel soldierModel("GameResource/nanosuit/nanosuit.obj");
	CModel planetModel("GameResource/planet/planet.obj");
	glm::vec3 planetPos = glm::vec3(0.0f, 0.0f, 0.0f);

	CModel asteroidModel("GameResource/rock/rock.obj");
	float radius = 150.0f;
	glm::vec3 asteroidPos = glm::vec3(-radius, 0.0f, 0.0f);

	//light position
	glm::vec3 lightPos = glm::vec3(100.0f, 100.0f, 100.0f);
	glm::vec3 lightColor = glm::vec3(0.5f, 0.5f, 0.5f);
	float shininess = 32.0f;

	//initialize camera
	CCamera3D camera(screenWidth, screenHeight,
		glm::vec3(0.0f, 0.0f, 150.0f),
		glm::vec3(0.0f, 0.0f, -1.0f));
	camera.SetSpeed(10.5f);
	camera.SetSensitivity(2.0f);

	//setup uniform buffer object
	/*
	option1:
	two uniform buffer objects

	uniform buffer
	layout (std140) uniform uboBuffer
	{
		 mat4 projection;    0
		 mat4 view;          64
	}

	layout (std140) uniform uboLight
	{
		vec4 lightPos;       0
		vec4 lightColor;     16
		vec4 eyePos;         32
		float shininess;     48
	}

	option2:
	one uniform buffer objects, be careful of the uniform buffer block alignment value

	layout (std140) uniform uboBuffer, block alignment = 256
	{
		mat4 projection;    0
		mat4 view;          64
		vec4 lightPos;       256
		vec4 lightColor;     272
		vec4 eyePos;         288
		float shininess;     304
	}

	*/

	int sizef;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &sizef);
	std::cout << "uniform buffer offset alignment: " << sizef << std::endl;

	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &sizef);
	std::cout << "max uniform block size: " << sizef << std::endl;

	GLuint ubo = 0;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, 512, NULL, GL_DYNAMIC_DRAW);	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//end of setting ubo

	GLuint matrixBindingPoint = 10;
	GLuint lightBindingPoint = 11;

	//bind matrices uniform block to location index 10, lights uniform block to location index 11
	GLuint planetMatrixIndex = glGetUniformBlockIndex(planetShader.GetID(), "plaMatrices");
	glUniformBlockBinding(planetShader.GetID(), planetMatrixIndex, matrixBindingPoint);
	GLuint planetLightIndex = glGetUniformBlockIndex(planetShader.GetID(), "plaLights");
	glUniformBlockBinding(planetShader.GetID(), planetLightIndex, lightBindingPoint);

	//bind matrices uniform block to location index 10, lights uniform block to location index 11
	GLuint asteroidUBOIndex = glGetUniformBlockIndex(asteroidShader.GetID(), "astMatrices");
	glUniformBlockBinding(asteroidShader.GetID(), asteroidUBOIndex, matrixBindingPoint);
	GLuint asteroidLightIndex = glGetUniformBlockIndex(asteroidShader.GetID(), "astLights");
	glUniformBlockBinding(asteroidShader.GetID(), asteroidLightIndex, lightBindingPoint);

	glBindBufferRange(GL_UNIFORM_BUFFER, matrixBindingPoint, ubo, 0, 128);
	glBindBufferRange(GL_UNIFORM_BUFFER, lightBindingPoint, ubo, 256, 62);

	//create projection matrix
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);
	//link projection matrix to ubo
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, 256, 16, glm::value_ptr(lightPos));
	glBufferSubData(GL_UNIFORM_BUFFER, 272, 16, glm::value_ptr(lightColor));
	glBufferSubData(GL_UNIFORM_BUFFER, 304, 4, &shininess);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//initial time tick
	Uint32 previous = SDL_GetTicks();
	Uint32 lag = 0;
	Uint32 MS_PER_FRAME = 8;

	while (isRunning)
	{
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
		glm::mat4 view = camera.GetCameraMatrix();
		glm::vec3 eyePos = camera.GetPosition();
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(view));
		glBufferSubData(GL_UNIFORM_BUFFER, 288, 16, glm::value_ptr(eyePos));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		////render
		glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer

		
		//render planet
		planetShader.Use();

		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, planetPos);
		model = glm::scale(model, glm::vec3(5.0, 5.0, 5.0));
		planetShader.SetUniformMat4("model", model);

		if (inputManager.IskeyPressed(SDLK_1))
			planetShader.SetUniformInt("eff", 1);
		if (inputManager.IskeyPressed(SDLK_2))
			planetShader.SetUniformInt("eff", 2);

		planetModel.Render(planetShader);

		planetShader.Unuse();
		//end of rendering planet

		//render asteroid
		asteroidShader.Use();

		glm::mat4 asteroidMode = glm::mat4(1.0);
		asteroidMode = glm::translate(asteroidMode, asteroidPos);
		asteroidMode = glm::scale(asteroidMode, glm::vec3(1.0, 1.0, 1.0));

		asteroidShader.SetUniformMat4("model", asteroidMode);

		if (inputManager.IskeyPressed(SDLK_1))
			asteroidShader.SetUniformInt("eff", 1);
		if (inputManager.IskeyPressed(SDLK_2))
			asteroidShader.SetUniformInt("eff", 2);

		asteroidModel.Render(asteroidShader);

		asteroidShader.Unuse();
		//end of rendering asteroid

		SDL_GL_SwapWindow(window);

	} // end of main loop

	SDL_GL_DeleteContext(context);

}

void AdvancedModelEx()
{
	//initial SDL
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow("Space", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);

	SDL_ShowCursor(0);

	//initialize glew
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


	//initialize shader instance
	CShader objectShader;

	//load vertex shader and fragment shader
	objectShader.AttachShader("Shaders/ObjectVertexShaderSpace.vert", "Shaders/ObjectFragmentShaderSpace.frag");

	//initialize crate object
	GLTexture objectTexture = CSTexture::LoadImage("GameResource/crates/crate.png");
	GLTexture objectSpecular = CSTexture::LoadImage("GameResource/crates/crate Specular.png");
	GLTexture objectEmission = CSTexture::LoadImage("GameResource/crates/crate emission.png");

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

	glm::vec3 objectPos = glm::vec3(0.0f, 0.0f, 0.0f);

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

	//light position
	glm::vec3 lightPos = glm::vec3(20.0f, 0.0f, 20.0f);
	glm::vec3 lightColor = glm::vec3(0.0f, 0.0f, 1.0f);
	float shininess = 16.0f;

	//initialize camera
	CCamera3D camera(screenWidth, screenHeight,
		glm::vec3(0.0f, 0.0f, 50.0f),
		glm::vec3(0.0f, 0.0f, -1.0f));
	camera.SetSpeed(10.5f);
	camera.SetSensitivity(2.0f);

	//setup uniform buffer object
	/*
	uniform buffer
	layout (std140) uniform uboBuffer, block alignment = 16
	{
		mat4 projection;    0
		mat4 view;          64
		vec4 lightPos;       128
		vec4 lightColor;     144
		vec4 eyePos;         160
		float shininess;     176
	}
	layout (std140) uniform uboBuffer, block alignment = 256
	{
		mat4 projection;    0
		mat4 view;          64
		vec4 lightPos;       256
		vec4 lightColor;     272
		vec4 eyePos;         288
		float shininess;     304
	}

	*/

	/*
		on single screen system this value is 16, 
		on two creens system it's 16 if main screen is embedded screen, it's 256 if main screen is extended screen  
	*/
	int sizef;
	glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &sizef);
	std::cout << "uniform buffer offset alignment: " << sizef << std::endl;

	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &sizef);
	std::cout << "max uniform block size: " << sizef << std::endl;

	GLuint ubo = 0;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	//glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBufferData(GL_UNIFORM_BUFFER, 512, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//end of setting ubo

	//bind matrices uniform block to location index 0, lights uniform block to location index 1
	GLuint planetMatrixIndex = glGetUniformBlockIndex(objectShader.GetID(), "Matrices");
	std::cout << "planet matrix index: " << planetMatrixIndex << std::endl;
	glUniformBlockBinding(objectShader.GetID(), planetMatrixIndex, 21);
	GLuint planetLightIndex = glGetUniformBlockIndex(objectShader.GetID(), "Lights");
	std::cout << "planet light index: " << planetLightIndex << std::endl;
	glUniformBlockBinding(objectShader.GetID(), planetLightIndex, 22);

	glBindBufferRange(GL_UNIFORM_BUFFER, 21, ubo, 0, 128);
	glBindBufferRange(GL_UNIFORM_BUFFER, 22, ubo, 256, 64);


	GLint uboSize = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glGetBufferParameteriv(GL_UNIFORM_BUFFER, GL_BUFFER_SIZE, &uboSize);
	std::cout << "ubo buffer size is: " << uboSize << std::endl;
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//create projection matrix
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);
	//link projection matrix to ubo
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, 256, 16, glm::value_ptr(lightPos));
	glBufferSubData(GL_UNIFORM_BUFFER, 272, 16, glm::value_ptr(lightColor));
	glBufferSubData(GL_UNIFORM_BUFFER, 304, 16, &shininess);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//initial time tick
	Uint32 previous = SDL_GetTicks();
	Uint32 lag = 0;
	Uint32 MS_PER_FRAME = 8;

	while (isRunning)
	{
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
		glm::mat4 view = camera.GetCameraMatrix();
		glm::vec3 eyePos = camera.GetPosition();
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(view));
		glBufferSubData(GL_UNIFORM_BUFFER, 288, 16, glm::value_ptr(eyePos));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		////render
		glEnable(GL_DEPTH_TEST);  //enable depth testing to the frame buffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //set background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear color buffer


		//render planet
		objectShader.Use();

		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, objectPos);
		model = glm::scale(model, glm::vec3(5.0, 5.0, 5.0));
		objectShader.SetUniformMat4("model", model);

		objectShader.SetUniformInt("texture_ambient", 1);
		objectShader.SetUniformInt("texture_diffuse", 2);
		objectShader.SetUniformInt("texture_specular", 3);
		objectShader.SetUniformInt("texture_emission", 4);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, objectTexture.ID);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, objectTexture.ID);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, objectSpecular.ID);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, objectEmission.ID);

		if (inputManager.IskeyPressed(SDLK_1))
			objectShader.SetUniformInt("eff", 1);
		if (inputManager.IskeyPressed(SDLK_2))
			objectShader.SetUniformInt("eff", 2);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		objectShader.Unuse();
		//end of rendering planet

		SDL_GL_SwapWindow(window);

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

void GetDebugInfo(GLuint pID)
{

}

int main(int args, char** argv)
{
	AdvancedModel();
	return 1;
}