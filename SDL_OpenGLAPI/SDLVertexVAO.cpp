
#include "SDL_OpenGLAPI.h"

extern GAMESTATE STATE;

//typical vertex shader and fragment shader usage
void SDLVAO()
{
	//init SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//SDL create window
	SDL_Window* window = SDL_CreateWindow("Test VAO", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 300, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	//init glew
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "Glew init error" << std::endl;
		exit(1);
	}


	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	glClearColor(0, 0, 0, 0);


	//shader source
	const char *pVertexShader = "#version 330\n"
		"layout (location = 0) in vec2 vertexPos;\n"
		"layout (location = 1) in vec3 vertexColor;\n"
		"out vec3 fragmentColor;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(vertexPos.x, vertexPos.y, 0, 1.0);\n"
		"fragmentColor = vertexColor;\n"
		"}\0";

	const char *pFragmentShader = "#version 330\n"
		"in vec3 fragmentColor;\n"
		"out vec4 color;\n"
		""
		"void main()\n"
		"{\n"
		"color = vec4(fragmentColor, 1.0);\n"
		"}\0";

	//shader log
	int shaderResult;
	char info[512];

	//complie vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	
	glShaderSource(vertexShader, 1, &pVertexShader, NULL);
	glCompileShader(vertexShader);
	
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderResult);
	
	if(!shaderResult)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, info);
		std::cout << "vertex shader compilation error: " << info << std::endl;
	}

	//compile fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &pFragmentShader, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderResult);

	if (!shaderResult)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, info);
		std::cout << "fragment shader compilation error: " << info << std::endl;
	}

	//link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderResult);
	if (!shaderResult)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, info);
		std::cout << "shader link error: " << info << std::endl;
	}

	//vertex
	GLuint vbo = 0;
	GLuint vao = 0;

	//generate and bind buffer
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	GLfloat vertice[] = { 
		-1.0f, -1.0f, 1.0, 0.0, 0.0, 
		0.0f, 1.0f, 0.0, 1.0, 0.0,
		1.0f, -1.0f, 0.0, 0.0, 1.0
	};

	//bind vertex array
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//it's allowed, glVertexAttribPointer() registered vbo as the vertex attribute's bound vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	


	//handle input
	while (STATE != GAMESTATE::EXIT)
	{
		ProcessInput();

		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(shaderProgram);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);


		SDL_GL_SwapWindow(window);
	}



	SDL_GL_DeleteContext(context);


}