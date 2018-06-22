#include "SDL_OpenGLAPI.h"

#include "stb_image.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

extern GAMESTATE STATE;

void SDLTransform()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("Transform Windows", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 500, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "Glew Initial failed" << std::endl;
		exit(1);
	}


	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	glClearColor(0, 0, 0, 0);


	//load texture
	int width, height, nrChannels;

	unsigned char* data = NULL;

	GLuint texture1;
	GLuint texture2;

	//flip the texture
	stbi_set_flip_vertically_on_load(true);

	//load texture1
	data = stbi_load("wall.jpg", &width, &height, &nrChannels, 0);
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Image load failed" << std::endl;
	}

	stbi_image_free(data);

	//load texture2, load png picture iwth GL_RGBA format
	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Image load failed" << std::endl;
	}

	stbi_image_free(data);



	//shader source
	const char *pVertexShader = "#version 330\n"
		"layout (location = 0) in vec2 vertexPos;\n"
		"layout (location = 1) in vec3 vertexColor;\n"
		"layout (location = 2) in vec2 vertexUV;\n"
		"out vec3 textureColor;\n"
		"out vec2 textureCoord;\n"
		"uniform mat4 transform;\n"
		"void main()\n"
		"{\n"
		"gl_Position = transform * vec4(vertexPos.x, vertexPos.y, 0, 1.0);\n"
		"textureColor = vertexColor;\n"
		"textureCoord = vertexUV;\n"
		"}\0";

	// for multiple textures
	const char *pFragmentShader = "#version 330\n"
		"in vec3 textureColor;\n"
		"in vec2 textureCoord;\n"
		"out vec4 color;\n"
		"uniform sampler2D sampler1;\n"
		"uniform sampler2D sampler2;\n"
		"void main()\n"
		"{\n"
		"color = mix(texture(sampler1, textureCoord), texture(sampler2, textureCoord), 0.3) * vec4(textureColor, 1.0f);\n"
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

	//assign the texture unit
	glUseProgram(shaderProgram);

	GLint location = 0;
	location = glGetUniformLocation(shaderProgram, "sampler1");
	glUniform1i(location, 1);

	location = glGetUniformLocation(shaderProgram, "sampler2");
	glUniform1i(location, 2);


	glUseProgram(0);


	GLuint vbo = 0;
	GLuint vao = 0;
	GLuint ebo = 0;

	//the rectangle 
	GLfloat vertice[] = {
		//Pos          //color            //uv
		-1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0, 0.0,  //bottom left
		-1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0, 1.0,  //top left
		 0.0f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0, 1.0,  //top right
		 0.0f, -1.0f,  1.0f, 1.0f, 0.0f, 1.0, 0.0   //bottom right
	};
	GLuint indices[] = { 0, 1, 2, 2, 3, 0 };

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (void*)(sizeof(GLfloat) * 2));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (void*)(sizeof(GLfloat) * 5));
	glEnableVertexAttribArray(2);


	//it's allowed, vbo is stored as vao's registered buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	//unbind ebo after unbinding vao is allowed
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	while (STATE != GAMESTATE::EXIT)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ProcessInput();
		
		//create tranfrom matrix
		glm::mat4 transform;
		//translate
		transform = glm::translate(transform, glm::vec3(0.5f, 0.5f, 0.0f));
		//rotate
		transform = glm::rotate(transform, (float)SDL_GetTicks()/1000.0f, glm::vec3(0.0, 0.0, 1.0));
		//scale
		transform = glm::scale(transform, glm::vec3(0.5, 0.5, 0.5));
		//translate to origin (0,0)
		transform = glm::translate(transform, glm::vec3(0.5, 0.5, 0));

		glUseProgram(shaderProgram);

		//assign transform location
		GLuint location = glGetUniformLocation(shaderProgram, "transform");
		glUniformMatrix4fv(location, 1, GL_FALSE, &transform[0][0]);

		//bind texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture2);


		//draw vertices
		glBindVertexArray(vao);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

		glUseProgram(0);


		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);


}