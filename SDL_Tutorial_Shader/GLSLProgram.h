#pragma once

#include <string>
#include <GLEW/glew.h>

class GLSLProgram
{
public:
	GLSLProgram();
	~GLSLProgram();

	void CompileShaders(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
	void LinkShaders();
	void AddAttribute(const std::string& attributeName);

	GLint GetUniformLocation(const std::string& uniformName);

	void use();
	void unuse();

private:
	void CompileSingleShader(const std::string& shaderFilePath, GLuint id);
	GLuint m_iProgramID;
	GLuint m_iVertexShaderID;
	GLuint m_iFragmentShaderID;
	int m_iNumAttributes;
};

