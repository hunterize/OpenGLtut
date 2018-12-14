#include "SDL_OpenGLAPI.h"

#include "stb_image.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

extern GAMESTATE STATE;

namespace
{
	void GetInput();
	//camera 
	GLfloat cameraSpeed = 250.0f;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 50.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	//time span
	GLfloat previousTime = SDL_GetTicks() / 1000.0f;
	GLfloat deltaTime = 0.0f;
	bool isFirstMove = true;

	GLfloat rotationAngle = 0.0f;

	GLfloat yaw = -90.0f; //pointing to the nagtive Z axis as initial cameraFront (0, 0, -1)
	GLfloat pitch = 0.0f;
	GLfloat fov = 45.0f;

	GLfloat cameraMoveFactor = 0.3f;

}

void SDLCameraFPS()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("CameraFPS Windows", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 700, 500, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		std::cout << "Glew Initial failed" << std::endl;
		exit(1);
	}

	//enable double buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//enable depth testing
	glEnable(GL_DEPTH_TEST);

	//set background color
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
		"layout (location = 0) in vec3 vertexPos;\n"
		"layout (location = 1) in vec3 vertexColor;\n"
		"layout (location = 2) in vec2 vertexUV;\n"
		"out vec3 textureColor;\n"
		"out vec2 textureCoord;\n"
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vertexPos, 1.0);\n"
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
		//Pos                //color            //uv
		//front side
		-0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
		-0.5f, 0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
		0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
		0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0,   //bottom right
		//back side
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
		-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
		0.5f,  0.5f,  -0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
		0.5f, -0.5f,  -0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0,   //bottom right
		//left side
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
		-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
		-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0,   //bottom right
		//right side
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
		0.5f, 0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
		0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
		0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0,   //bottom right
		//top side
		-0.5f, 0.5f,  -0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
		0.5f,  0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
		0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
		-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0,   //bottom right
		//bottom side
		-0.5f, -0.5f,  -0.5f,  1.0f, 0.0f, 0.0f,  0.0, 0.0,  //bottom left
		0.5f,  -0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,  0.0, 1.0,  //top left
		0.5f,  -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0, 1.0,  //top right
		-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  1.0, 0.0   //bottom right
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
		glm::vec3(8.0, 8.0, 5.0),
		glm::vec3(0, 15.0, 0),
		glm::vec3(-5.0, -5.0, -2.0),
		glm::vec3(-10.0, -7.0, 5.0),
		glm::vec3(20.0, 10.0, -5.0),
		glm::vec3(10.0, -6.0, 4.0),
		glm::vec3(-7.0, 8.0, 4.0),
		glm::vec3(-8.0, 6.0, -6.0),
		glm::vec3(6.0, 3.0, 4.0)
	};

	//set vao
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (void*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);


	//it's allowed, vbo is stored as vao's registered buffer object
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	//unbind ebo after unbinding vao is allowed
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	while (STATE != GAMESTATE::EXIT)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//calculate delta time
		GLfloat currentTime = SDL_GetTicks() / 1000.0f;
		deltaTime = currentTime - previousTime;
		previousTime = currentTime;
		//cameraSpeed = deltaTime * 170.0f;
		rotationAngle += (2.0f) * deltaTime;

		//std::cout << "delta time: " << deltaTime << std::endl;
		//std::cout << "camera speed: " << cameraSpeed << std::endl;

		//handle input and update 
		::GetInput();

		//create projection matrix, 
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(fov), (float)700 / 500, 1.0f, 1000.0f);

		glUseProgram(shaderProgram);

		GLuint projectionlocation = glGetUniformLocation(shaderProgram, "projection");
		glUniformMatrix4fv(projectionlocation, 1, GL_FALSE, &projection[0][0]);

		//draw vertices
		glBindVertexArray(vao);

		//bind texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture2);

		//create camera matrix
		glm::mat4 view;
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		//assign view location
		GLuint viewlocation = glGetUniformLocation(shaderProgram, "view");
		glUniformMatrix4fv(viewlocation, 1, GL_FALSE, &view[0][0]);

		for (int i = 0; i < sizeof(obj_pos) / sizeof(glm::vec3); i++)
		{
			//create tranfrom matrix
			glm::mat4 model;
			//translate
			model = glm::translate(model, obj_pos[i]);
			//rotate
			model = glm::rotate(model, (GLfloat)glm::pow(-1, i) * rotationAngle, glm::vec3(0.5, 1.0, 0.0));
			//scale
			model = glm::scale(model, glm::vec3(4, 4, 4));

			//assign transform location
			GLuint location = glGetUniformLocation(shaderProgram, "model");
			glUniformMatrix4fv(location, 1, GL_FALSE, &model[0][0]);

			//draw vertices
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		}

		glBindVertexArray(0);

		glUseProgram(0);

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
}

namespace
{
	void GetInput()
	{
		SDL_Event event;

		//SDL_WarpMouseInWindow(NULL, 700 / 2, 500 / 2);
		//lastX = 700 / 2;
		//lastY = 500 / 2;

		while (SDL_PollEvent(&event))
		{

			switch (event.type)
			{
			case SDL_QUIT:
				STATE = GAMESTATE::EXIT;
				break;

			case SDL_WINDOWEVENT:

				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_LEAVE:
					std::cout << "Focus lost" << std::endl;
					isFirstMove = true;
				default:
					break;
				}


				break;
			
			case SDL_KEYDOWN:
				//std::cout << event.key.keysym.sym << std::endl;
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					STATE = GAMESTATE::EXIT;
					break;
				case SDLK_a:
					cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
					break;
				case SDLK_s:
					cameraPos -= cameraFront * cameraSpeed * deltaTime;
					break;
				case SDLK_d:
					cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
					break;
				case SDLK_w:
					cameraPos += cameraFront * cameraSpeed * deltaTime;
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
					std::cout << "LEFT BUTTON" << std::endl;
					break;
				case SDL_BUTTON_RIGHT:
					std::cout << "RIGHT BUTTTON" << std::endl;
					break;
				case SDL_BUTTON_MIDDLE:
					std::cout << "MIDDLE BUTTON" << std::endl;
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				//std::cout << event.motion.x << " " << event.motion.y << std::endl;

				if (isFirstMove)
				{
					SDL_WarpMouseInWindow(NULL, 700 / 2, 500 / 2);
					isFirstMove = false;
				}
				else
				{

					GLfloat offsetX, offsetY;

					offsetX = event.motion.x - 700 / 2;
					offsetY = event.motion.y - 500 / 2;

					std::cout << offsetX << " " << offsetY << std::endl;

					yaw += offsetX * 180 / 700 * cameraMoveFactor;
					pitch -= offsetY * 180 / 500 * cameraMoveFactor;

					pitch = pitch > 89.0f ? 89.0f : pitch;
					pitch = pitch < -89.0f ? -89.0f : pitch;

					glm::vec3 direction;

					direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
					direction.y = sin(glm::radians(pitch));
					direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

					cameraFront = glm::normalize(direction);

				}
				SDL_WarpMouseInWindow(NULL, 700 / 2, 500 / 2);
				break;

			case SDL_MOUSEWHEEL:
				if (fov >= 1.0f && fov <= 45.0f)
				{
					fov -= event.wheel.y;
				}

				if (fov <= 1.0f)
				{
					fov = 1.0f;
				}

				if (fov >= 45.0f)
				{
					fov = 45.0f;
				}
				break;
			}//end of switch
		}//end of while

	}
}