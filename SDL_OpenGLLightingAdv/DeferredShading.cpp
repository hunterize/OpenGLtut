#include "SDL_OpenGLLightingAdv.h"

namespace DeferredShading
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

	/*
		const GLfloat constant = 1.0f;
		const GLfloat linear = 0.7f;
		const GLfloat quadratic = 0.08f;
	*/

	struct CLight
	{
		glm::vec3 m_position;
		glm::vec3 m_color;
		GLfloat m_radius;
		GLfloat m_dIntensity = 0.3f;
		GLfloat m_sIntensity = 0.6f;
		struct 
		{
			GLfloat constant = 1.0f;
			GLfloat linear = 0.7f;
			GLfloat quadratic = 0.08f;
			GLfloat factor = 0.01f;
		} m_attenuation;
	};

	int InitLightRadius(CLight& light)
	{
		const GLfloat brightness = std::fmaxf(std::fmaxf(light.m_color.x, light.m_color.y), light.m_color.z);
		
		light.m_attenuation.factor = brightness * 256.0f / 1.0f;
		light.m_radius = (-light.m_attenuation.linear + 
			std::sqrtf(
				light.m_attenuation.linear * light.m_attenuation.linear - 
				4.0f * light.m_attenuation.quadratic * (light.m_attenuation.constant - light.m_attenuation.factor))) 
			/ (2.0f * light.m_attenuation.quadratic);


		return 1;
	}


	void DeferredShading()
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
			glm::vec3(180.0f, 180.0f, 180.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f));

		camera.SetSpeed(30.0f);

		window = SDL_CreateWindow("DeferredShading", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
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

		int usize;
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &usize);
		std::cout << "Max Vertex Uniform Components: " << usize << std::endl;
		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &usize);
		std::cout << "Max Fragment Uniform Components: " << usize << std::endl;
		glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &usize);
		std::cout << "Stencil clear value: " << usize << std::endl;


		//initialize vertices for game object
		GLuint cubeVBO = 0;
		GLuint cubeVAO = 0;
		GLuint cubeEBO = 0;

		//the cube, counterclock wise vertices
		GLfloat crateVertices[] = {
			//Pos                 //normal				//tangent           //bitangent         //texture coordinates
			//front side
			-0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,		//bottom left
			0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,		//bottom right
			0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,		//top right
			-0.5f, 0.5f,  0.5f,   0.0f, 0.0f, 1.0f,		1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,		//top left			
			//back side
			0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0,		-1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,	0.0f, 0.0f,		//bottom left
			-0.5f, -0.5f,  -0.5f,  0.0f, 0.0f, -1.0,	-1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,	1.0f, 0.0f,		//bottom right
			-0.5f, 0.5f,  -0.5f,  0.0f, 0.0f, -1.0,		-1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,	1.0f, 1.0f,		//top right
			0.5f, 0.5f,  -0.5f,   0.0f, 0.0f, -1.0,		-1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,	0.0f, 1.0f,		//top left			
			//left side
			-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	0.0f, 0.0f,		//bottom left
			-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	1.0f, 0.0f,		//bottom right
			-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	1.0f, 1.0f,		//top right
			-0.5f, 0.5f,  -0.5f,  -1.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	0.0f, 1.0f,		//top left			
			//right side
			0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,	0.0f, 0.0f,		//bottom left
			0.5f, -0.5f,  -0.5f,  1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,	1.0f, 0.0f,		//bottom right
			0.5f,  0.5f,  -0.5f,  1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,	1.0f, 1.0f,		//top right
			0.5f, 0.5f,   0.5f,   1.0f, 0.0f, 0.0f,		0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,	0.0f, 1.0f,		//top left			
			//top side
			-0.5f, 0.5f,  0.5f,  0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,	0.0f, 0.0f,		//bottom left
			0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,	1.0f, 0.0f,		//bottom right
			0.5f,  0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,	1.0f, 1.0f,		//top right
			-0.5f, 0.5f,  -0.5f,  0.0f, 1.0f, 0.0f,		1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,	0.0f, 1.0f,		//top left			
			//bottom side
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,	1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,	0.0f, 0.0f,		//bottom left
			0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,	1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,	1.0f, 0.0f,		//bottom right
			0.5f,  -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,	1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,	1.0f, 1.0f,		//top right
			-0.5f,  -0.5f, 0.5f,  0.0f, -1.0f, 0.0f,	1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,	0.0f, 1.0f		//top left			
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
		glGenBuffers(1, &cubeVBO);
		glGenBuffers(1, &cubeEBO);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(crateVertices), crateVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(crateIndices), crateIndices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//set attribute location for vertex arrays
		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);

		//Attribute location = 0 in vertex shader
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)0);
		glEnableVertexAttribArray(0);
		//Attribute location = 1 in vertex shader
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);
		//Attribute location = 2 in vertex shader
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 6));
		glEnableVertexAttribArray(2);
		//Attribute location = 3 in vertex shader
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 9));
		glEnableVertexAttribArray(3);
		//Attribute location = 4 in vertex shader
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 14, (void*)(sizeof(GLfloat) * 12));
		glEnableVertexAttribArray(4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		//end of crate vao

		//vertices for screen rendering 
		GLuint screenVAO = 0;
		GLuint screenVBO = 0;

		GLfloat screenVertices[] = {
			//position          //uv
			-1.0f, 1.0f,        0.0f, 1.0f,  //top left
			-1.0f, -1.0f,       0.0f, 0.0f,  //bottom left
			1.0f,  -1.0f,       1.0f, 0.0f,  //bottom right
			1.0f,  -1.0f,       1.0f, 0.0f,  //bottom right
			1.0f,  1.0f,        1.0f, 1.0f,  //top right
			-1.0f, 1.0f,        0.0f, 1.0f   //top left
		};

		glGenBuffers(1, &screenVBO);
		glGenVertexArrays(1, &screenVAO);

		glBindVertexArray(screenVAO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(sizeof(GLfloat) * 2));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		//end of screen vertices

		//vertices for point light
		GeometryFactory::GMesh plMesh;
		GeometryFactory::GGenerator::GetSphere(30, 30, plMesh);

		GLuint vaoPL;
		GLuint vboPL;
		GLuint eboPL;

		glGenBuffers(1, &vboPL);
		glGenBuffers(1, &eboPL);

		glBindBuffer(GL_ARRAY_BUFFER, vboPL);
		glBufferData(GL_ARRAY_BUFFER, plMesh.m_vertices.size() * sizeof(GeometryFactory::GVertex), &plMesh.m_vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboPL);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, plMesh.m_indices.size() * sizeof(GLuint), &plMesh.m_indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//set attribute location for vertex arrays
		glGenVertexArrays(1, &vaoPL);
		glBindVertexArray(vaoPL);
		glBindBuffer(GL_ARRAY_BUFFER, vboPL);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboPL);

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

		//end of vertices for point light

		//config G-buffer
		GLuint gBufferFBO;
		GLuint gBufferRBO;
		glGenFramebuffers(1, &gBufferFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

		unsigned int gbPosition, gbNormal, gbAlbedo, gbScene;

		//position g-buffer
		glGenTextures(1, &gbPosition);
		glBindTexture(GL_TEXTURE_2D, gbPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbPosition, 0);

		//normal g-buffer
		glGenTextures(1, &gbNormal);
		glBindTexture(GL_TEXTURE_2D, gbNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbNormal, 0);

		//albedo g-buffer, use RGBA format, alpha channel is for specular intensity
		glGenTextures(1, &gbAlbedo);
		glBindTexture(GL_TEXTURE_2D, gbAlbedo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gbAlbedo, 0);

		//final scene g-buffer, use RGB format
		glGenTextures(1, &gbScene);
		glBindTexture(GL_TEXTURE_2D, gbScene);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gbScene, 0);

		//unsigned int gbAttachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		//glDrawBuffers(3, gbAttachments);

		//set render buffer for depth and stencil
		glGenRenderbuffers(1, &gBufferRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, gBufferRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gBufferRBO);

		//check if the frame buffer with attachment is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Frame buffer is not complete!" << std::endl;
			exit(0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//end of configing g-buffer

		//config shaders
		CShader objShader;
		objShader.AttachShader("Shaders/DeferredGeometryVertexShader.vert", "Shaders/DeferredGeometryFragmentShader.frag");
		//CShader lightShader;
		//lightShader.AttachShader("Shaders/DeferredLightVertexShader.vert", "Shaders/DeferredLightFragmentShader.frag");
		//CShader screenShader;
		//screenShader.AttachShader("Shaders/DeferredScreenVertexShader.vert", "Shaders/DeferredScreenFragmentShader.frag");

		CShader stencilShader;
		stencilShader.AttachShader("Shaders/DeferredStencilVertexShader.vert", "Shaders/DeferredStencilFragmentShader.frag");

		CShader plShader;
		plShader.AttachShader("Shaders/DeferredPointLightVertexShader.vert", "Shaders/DeferredPointLightFragmentShader.frag");

		GLTexture crateTexture = CSTexture::LoadImage("brickwall.jpg");
		GLTexture crateNormal = CSTexture::LoadImage("brickwall_normal.jpg");

		//create projection matrix
		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)screenWidth / screenHeight, 0.1f, 1000.0f);

		glm::vec3 pointLightPos = glm::vec3(30.0f, 20.0f, 30.0f);

		//1000 crates
		std::vector<glm::vec3> cratesPos;
		cratesPos.reserve(1000);

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				for(int k = 0; k < 10; k++)
				{
					glm::vec3 pos = glm::vec3(150.0f - 30.0f * i, 150.0f - 30.0f * j, 150.0f - 30.0f * k);
					cratesPos.push_back(pos);
				}
			}
		}

		glm::vec3 obj[] = {
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 30.0f, 0.0f)
		};

		//1331 lights
		std::vector<CLight> cubeLights;
		cubeLights.reserve(1331);

		const GLfloat constant = 1.0f;
		const GLfloat linear = 0.7f;
		const GLfloat quadratic = 1.8f;

		
		for (int i = 0; i < 11; i++)
		{
			for (int j = 0; j < 11; j++)
			{
				for (int k = 0; k < 11; k++)
				{
					CLight light;
					light.m_position = glm::vec3(165.0f - 30.0f * i, 165.0f - 30.0f * j, 165.0f - 30.0f * k); 
					light.m_color	= glm::vec3(0.09f + 0.09f * i, 0.09f + 0.09f * j, 0.09f + 0.09f * k);
					InitLightRadius(light);
					cubeLights.push_back(light);
				}
			}
		}
		

		/*
		{
			CLight light;
			light.m_position = glm::vec3(165.0f, 165.0f, 165.0 - 30.0);
			light.m_color = glm::vec3(0.0, 0.0, 1.0f);
			InitLightRadius(light);
			cubeLights.push_back(light);

			light.m_position = glm::vec3(165.0f - 30.0f, 165.0f, 165.0f);
			light.m_color = glm::vec3(1.0f, 0.0, 0.0);
			InitLightRadius(light);
			cubeLights.push_back(light);

			light.m_position = glm::vec3(165.0f, 165.0 - 30.0f, 165.0f);
			light.m_color = glm::vec3(0.0f, 1.0f, 0.0f);
			InitLightRadius(light);
			cubeLights.push_back(light);
		}
		*/

		GLfloat exposure = 1.0f;
		bool isDebug = false;
		bool isBlur = false;

		//initial time tick
		Uint32 previous = SDL_GetTicks();
		Uint32 lag = 0;
		Uint32 MS_PER_FRAME = 8;

		while (isRunning)
		{
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//glEnable(GL_DEPTH_TEST);  //enable depth testing

			Uint32 current = SDL_GetTicks();

			Uint32 elapsed = current - previous;
			previous = current;
			lag = elapsed;
			//Uint32 lag = elapsed;

			float timespan = (float)MS_PER_FRAME / 1000;

			//update input
			ProcessInput();
			camera.Update(inputManager, (float)elapsed / 1000);

			/*
			if (lag >= MS_PER_FRAME)
			{
				//update game
				//update camera
				camera.Update(inputManager, timespan);

				lag -= MS_PER_FRAME;
			}
			else
			{
				//synchronize the update and render
				Uint32 step = lag % MS_PER_FRAME;
				camera.Update(inputManager, (float)step / 1000);
				lag = 0;
			}
			*/

			//create view matrix
			glm::mat4 view = camera.GetCameraMatrix();

			glm::vec3 eyePos = camera.GetPosition();

			glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
			glDrawBuffer(GL_COLOR_ATTACHMENT3);
			glClear(GL_COLOR_BUFFER_BIT);

			//Geometry Pass

			//bind g-buffer frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

			//change draw buffers of G buffer
			unsigned int gbAttachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(3, gbAttachments);

			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
			//glDepthFunc(GL_LESS);
			
			//glDisable(GL_STENCIL_TEST);
			//glDisable(GL_BLEND);

			//render crates
			objShader.Use();
			objShader.SetUniformMat4("projection", projection);
			objShader.SetUniformMat4("view", view);

			objShader.SetUniformInt("sample", 20);
			glActiveTexture(GL_TEXTURE20);
			glBindTexture(GL_TEXTURE_2D, crateTexture.ID);

			objShader.SetUniformInt("nmap", 21);
			glActiveTexture(GL_TEXTURE21);
			glBindTexture(GL_TEXTURE_2D, crateNormal.ID);

			for (int i = 0; i < cratesPos.size(); i++)
			{
				glm::mat4 model;
				model = glm::translate(model, cratesPos[i]);
				model = glm::scale(model, glm::vec3(15.0f, 15.0f, 15.0f));

				objShader.SetUniformMat4("model", model);
				glBindVertexArray(cubeVAO);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}

			glDepthMask(GL_FALSE);
			
			objShader.Unuse();
			//end of rendering cubes

			glEnable(GL_STENCIL_TEST);

			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);

			//iterate all lights
			for (int i = 0; i < cubeLights.size(); i++)
			{
				//Stencil Pass
				//stencil pass is till on g-buffer framebuffer 
				glDrawBuffer(GL_NONE);
				glDisable(GL_CULL_FACE);
				glClear(GL_STENCIL_BUFFER_BIT);

				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);
				glDepthMask(GL_FALSE);

				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
				glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
				
				stencilShader.Use();

				glm::mat4 model;
				model = glm::translate(model, cubeLights[i].m_position);
				model = glm::scale(model, glm::vec3(cubeLights[i].m_radius));

				stencilShader.SetUniformMat4("model", model);
				stencilShader.SetUniformMat4("view", view);
				stencilShader.SetUniformMat4("projection", projection);

				glBindVertexArray(vaoPL);
				glDrawElements(GL_TRIANGLES, plMesh.m_indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);

				stencilShader.Unuse();
				//glDepthMask(GL_TRUE);

				//Light Pass
				glDrawBuffer(GL_COLOR_ATTACHMENT3);

				glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

				glDisable(GL_DEPTH_TEST);

				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);

				plShader.Use();
				plShader.SetUniformMat4("model", model);
				plShader.SetUniformMat4("view", view);
				plShader.SetUniformMat4("projection", projection);

				plShader.SetUniformVec3("uLight.position", cubeLights[i].m_position);
				plShader.SetUniformVec3("uLight.color", cubeLights[i].m_color);
				plShader.SetUniformFloat("uLight.dIntensity", cubeLights[i].m_dIntensity);
				plShader.SetUniformFloat("uLight.sIntensity", cubeLights[i].m_sIntensity);
				plShader.SetUniformFloat("uLight.attenuation.constant", cubeLights[i].m_attenuation.constant);
				plShader.SetUniformFloat("uLight.attenuation.linear", cubeLights[i].m_attenuation.linear);
				plShader.SetUniformFloat("uLight.attenuation.quadratic", cubeLights[i].m_attenuation.quadratic);
				plShader.SetUniformFloat("uLight.attenuation.factor", cubeLights[i].m_attenuation.factor);

				plShader.SetUniformVec3("uEyePos", eyePos);
				plShader.SetUniformFloat("shininess", 128.0f);
				plShader.SetUniformVec2("uScreenSize", glm::vec2(screenWidth, screenHeight));

				plShader.SetUniformInt("gPosition", 10);
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_2D, gbPosition);

				plShader.SetUniformInt("gNormal", 11);
				glActiveTexture(GL_TEXTURE11);
				glBindTexture(GL_TEXTURE_2D, gbNormal);

				plShader.SetUniformInt("gAlbedoSpecular", 12);
				glActiveTexture(GL_TEXTURE12);
				glBindTexture(GL_TEXTURE_2D, gbAlbedo);

				glBindVertexArray(vaoPL);
				glDrawElements(GL_TRIANGLES, plMesh.m_indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);

				plShader.Unuse();

				glCullFace(GL_BACK);


			}

			glDisable(GL_BLEND);
			glDisable(GL_STENCIL_TEST);

			//glDisable(GL_STENCIL_TEST);

			//swap to default frame buffer
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBO);
			glReadBuffer(GL_COLOR_ATTACHMENT3);
			glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			SDL_GL_SwapWindow(window);

			/*
			if (elapsed < 16)
			{
				SDL_Delay(16 - elapsed);
			}
			*/
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