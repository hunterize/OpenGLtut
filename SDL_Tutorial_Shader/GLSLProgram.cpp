#include "GLSLProgram.h"
#include "Errors.h"

#include <fstream>
#include <vector>

GLSLProgram::GLSLProgram()
{
	m_iProgramID = 0;
	m_iFragmentShaderID = 0;
	m_iVertexShaderID = 0;
	m_iNumAttributes = 0;
}


GLSLProgram::~GLSLProgram()
{
}

void GLSLProgram::CompileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{

	m_iProgramID = glCreateProgram();


	m_iVertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	if (m_iVertexShaderID == 0)
	{
		fatalError("Vertex shader failed to be created");
	}

	m_iFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	if (m_iFragmentShaderID == 0)
	{
		fatalError("Fragment shader failed to be created");
	}

	CompileSingleShader(vertexShaderFilePath, m_iVertexShaderID);
	CompileSingleShader(fragmentShaderFilePath, m_iFragmentShaderID);
}

void GLSLProgram::CompileSingleShader(const std::string& filePath, GLuint id)
{
	std::ifstream vertexFile(filePath);
	if (vertexFile.fail())
	{
		perror(filePath.c_str());
		fatalError("Failed to open " + filePath);
	}

	std::string fileContents = "";
	std::string line;

	while (std::getline(vertexFile, line))
	{
		fileContents += line + "\n";
	}

	vertexFile.close();

	const char* pContents = fileContents.c_str();

	glShaderSource(id, 1, &pContents, nullptr);

	glCompileShader(id);

	GLint success = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<char> errorlog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &errorlog[0]);


		glDeleteShader(id);

		std::printf("%s\n", &(errorlog[0]));
		fatalError("Shader" + filePath + "failed to compile");

	}

}


void GLSLProgram::LinkShaders()
{

	glAttachShader(m_iProgramID, m_iVertexShaderID);
	glAttachShader(m_iProgramID, m_iFragmentShaderID);

	glLinkProgram(m_iProgramID);


	GLint isLinked = 0;
	glGetProgramiv(m_iProgramID, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(m_iProgramID, GL_INFO_LOG_LENGTH, &maxLength);


		std::vector<char> infoLog(maxLength);
		glGetProgramInfoLog(m_iProgramID, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(m_iProgramID);
		glDeleteShader(m_iVertexShaderID);
		glDeleteShader(m_iFragmentShaderID);

		std::printf("%s\n", &(infoLog[0]));
		fatalError("Shader failed to link");

	}

	glDetachShader(m_iProgramID, m_iVertexShaderID);
	glDetachShader(m_iProgramID, m_iFragmentShaderID);

	glDeleteShader(m_iVertexShaderID);
	glDeleteShader(m_iFragmentShaderID);


}

void GLSLProgram::AddAttribute(const std::string& attributeName)
{
	glBindAttribLocation(m_iProgramID, m_iNumAttributes++, attributeName.c_str());
}

GLint GLSLProgram::GetUniformLocation(const std::string& uniformName)
{
	GLint location = glGetUniformLocation(m_iProgramID, uniformName.c_str());
	if (location == GL_INVALID_INDEX)
	{
		fatalError("uniform" + uniformName + "not found in shader");

	}
	return location;
}

void GLSLProgram::use()
{
	glUseProgram(m_iProgramID);
	for (int i = 0; i < m_iNumAttributes; i++)
	{
		glEnableVertexAttribArray(i);
	}
}

void GLSLProgram::unuse()
{
	glUseProgram(0);
	for (int i = 0; i < m_iNumAttributes; i++)
	{
		glDisableVertexAttribArray(i);
	}
}