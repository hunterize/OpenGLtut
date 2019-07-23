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

void CShader::SetUniformInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_iProgramID, name.c_str()), value);
}
void CShader::SetUniformFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_iProgramID, name.c_str()), value);
}
void CShader::SetUniformVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(m_iProgramID, name.c_str()), 1, &value[0]);
}
void CShader::SetUniformVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(m_iProgramID, name.c_str()), 1, &value[0]);
}
void CShader::SetUniformVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(m_iProgramID, name.c_str()), 1, &value[0]);
}
void CShader::SetUniformMat4(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_iProgramID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

GLuint CShader::GetID()
{
	return m_iProgramID;
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