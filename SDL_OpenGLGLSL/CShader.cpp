#include "CShader.h"



CShader::CShader()
{
}


CShader::~CShader()
{
}

void CShader::AttachShader(const char* vertexShaderFilePath, const char* fragmentShaderFilePath)
{
	m_iProgramID = glCreateProgram();

	m_iVertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	if (m_iVertexShaderID == 0)
	{
		std::cout << "Create Vertext Shader Failed" << std::endl;
		return;
	}
	m_iFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	if (m_iFragmentShaderID == 0)
	{
		std::cout << "Create Fragment Shader Failed" << std::endl;
		return;
	}

	//compile shader
	CompileShader(vertexShaderFilePath, SHADERTYPE::VERTEXSHADER, m_iVertexShaderID);
	CompileShader(fragmentShaderFilePath, SHADERTYPE::FRAGMENTSHADER, m_iFragmentShaderID);


	//link shader
	glAttachShader(m_iProgramID, m_iVertexShaderID);
	glAttachShader(m_iProgramID, m_iFragmentShaderID);

	glLinkProgram(m_iProgramID);

	if (!CheckCompileError(m_iProgramID, SHADERTYPE::PROGRAM))
	{
		std::cout << "Link Program Failed" << std::endl;
		glDeleteProgram(m_iProgramID);
		return;
	}


}

GLint CShader::GetUniformLocation(const std::string& uniformName)
{
	GLint location = glGetUniformLocation(m_iProgramID, uniformName.c_str());
	if (location == GL_INVALID_INDEX)
	{
		std::cout << "Get Uniform Location Failed" << std::endl;
	}

	return location;
}

void CShader::Use()
{
	glUseProgram(m_iProgramID);

}
void CShader::Unuse()
{
	glUseProgram(0);
}

void CShader::Dispose()
{
	if (m_iProgramID)
	{
		glDeleteProgram(m_iProgramID);
	}
}


bool CShader::CompileShader(const char* filePath, SHADERTYPE type, GLuint id)
{
	//get shader file content
	std::string shaderString;

	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		shaderFile.open(filePath);
		

		std::stringstream sStream;
		sStream << shaderFile.rdbuf();

		shaderFile.close();

		shaderString = sStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Read Shader File Failed" << std::endl;
		return false;
	}

	//compile shader 
	const char* pRawData = shaderString.c_str();

	glShaderSource(id, 1, &pRawData, nullptr);
	glCompileShader(id);

	if (!CheckCompileError(id, type))
	{
		glDeleteShader(id);
		return false;
	}
	
	return true;
}

bool CShader::CheckCompileError(GLuint id, SHADERTYPE type)
{
	int result;
	char info[1024];

	if (type == SHADERTYPE::PROGRAM)
	{
		glGetProgramiv(id, GL_LINK_STATUS, &result);
		if (!result)
		{
			glGetProgramInfoLog(id, 1024, nullptr, info);
			std::cout << "Shader Linking Failed: " << info << std::endl;
			return result;
		}

	}
	else
	{
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			glGetShaderInfoLog(id, 1024, nullptr, info);
			std::cout << "Shader Compiling Failed: " << (type == SHADERTYPE::VERTEXSHADER ? "Vertex - " : "Fragment - ") << info << std::endl;
			return result;
		}
	}

	return true;
}