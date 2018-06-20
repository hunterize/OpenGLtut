#include "SDL_OpenGLAPI.h"

extern GAMESTATE STATE;

void SDLEBO()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("EBO Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 300, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum err;
	err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "glew initial failed" << std::endl;
		exit(1);
	}

	//shader source
	const char *pVertexShader = "#version 330\n"
		"layout (location = 0) in vec2 vertexPos;\n"
		"out vec2 fragmentPos;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(vertexPos.x, vertexPos.y, 0, 1.0);\n"
		"}\0";

	const char *pFragmentShader = "#version 330\n"
		"in vec2 fragmentPos;\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"color = vec4(1.0, 0, 0, 1.0);\n"
		"}\0";

	//shader log
	int shaderResult;
	char info[512];

	//complie vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &pVertexShader, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderResult);

	if (!shaderResult)
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


	GLuint vbo = 0;
	GLuint vao = 0;
	GLuint ebo = 0;

	GLfloat vertice[] = {-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f};
	GLuint indices[] = { 0, 1, 2, 2, 3, 0 };

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void*)0);
	glEnableVertexAttribArray(0);

	//it's allowed, vbo is stored as vao's registered buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//DO NOT unbind element array buffer while vao is active
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	//unbind ebo after unbinding vao is allowed
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	while (STATE == GAMESTATE::RUNNING)
	{
		ProcessInput();

		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		glBindVertexArray(vao);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glUseProgram(0);


		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
}