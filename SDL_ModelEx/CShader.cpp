#include "CShader.h"



CShader::CShader()
{
	m_iProgramID = glCreateProgram();
}


CShader::~CShader()
{
}

void CShader::AttachShader(const char* vertexShaderFilePath, const char* fragmentShaderFilePath, const char* geometryShaderFilePath)
{
	//compile shader
	if (vertexShaderFilePath != NULL)
	{
		m_iVertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		if (m_iVertexShaderID == 0)
		{
			std::cout << "Create Vertext Shader Failed" << std::endl;
			return;
		}
		CompileShader(vertexShaderFilePath, m_iVertexShaderID);
	}

	if(fragmentShaderFilePath != NULL)
	{
		m_iFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (m_iFragmentShaderID == 0)
		{
			std::cout << "Create Fragment Shader Failed" << std::endl;
			return;
		}
		CompileShader(fragmentShaderFilePath, m_iFragmentShaderID);
	}

	if (geometryShaderFilePath != NULL)
	{
		m_iGeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
		if (m_iGeometryShaderID == 0)
		{
			std::cout << "Create Geometry Shader Failed" << std::endl;
			return;
		}
		CompileShader(geometryShaderFilePath, m_iGeometryShaderID);
	}
	//link shader
	LinkShader();


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


bool CShader::CompileShader(const char* filePath, GLuint id)
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
		std::cout << "Read Shader File Failed: " << filePath << std::endl;
		return false;
	}

	//compile shader 
	const char* pRawData = shaderString.c_str();

	glShaderSource(id, 1, &pRawData, nullptr);
	glCompileShader(id);

	if (!CheckCompileError(id))
	{
		std::cout << "Shader File: " << filePath << std::endl;
		glDeleteShader(id);
		return false;
	}
	
	return true;
}

bool CShader::LinkShader()
{
	if (m_iVertexShaderID)
	{
		glAttachShader(m_iProgramID, m_iVertexShaderID);
	}
	if (m_iFragmentShaderID)
	{
		glAttachShader(m_iProgramID, m_iFragmentShaderID);
	}
	if (m_iGeometryShaderID)
	{
		glAttachShader(m_iProgramID, m_iGeometryShaderID);
	}

	glLinkProgram(m_iProgramID);

	if (!CheckLinkError(m_iProgramID))
	{
		std::cout << "Link Program Failed" << std::endl;
		glDeleteProgram(m_iProgramID);
		return false;
	}
	return true;
}

bool CShader::CheckCompileError(GLuint id)
{
	int result;
	char info[1024];

	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(id, 1024, nullptr, info);
		std::cout << "Shader Compiling Failed: " << info << std::endl;
		return result;
	}
	
	return true;
}

bool CShader::CheckLinkError(GLuint id)
{
	int result;
	char info[1024];

	glGetProgramiv(id, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(id, 1024, nullptr, info);
		std::cout << "Shader Linking Failed: " << info << std::endl;
		return result;
	}

	return true;
}