#pragma once

#include <string>
#include <glew.h>

#include <fstream>
#include <sstream>
#include <iostream>

enum class SHADERTYPE
{
	PROGRAM,
	VERTEXSHADER,
	FRAGMENTSHADER
};

/*
	NOT USE attribute functions to bind attrib in shaders by glBinderAttribLocation()
	attributes locations in shader file like
	...
	"layout (location = 0) in vec3 vertexPos;\n"
	"layout (location = 1) in vec3 vertexColor;\n"
	"layout (location = 2) in vec2 vertexUV;\n"
	...
*/


class CShader
{
public:
	CShader();
	~CShader();

	void AttachShader(const char* vertexShaderFilePath, const char* fragmentShaderPath);
	GLint GetUniformLocation(const std::string& uniformName);

	void Use();
	void Unuse();

	void Dispose();

private:
	bool CompileShader(const char* filePath, SHADERTYPE type, GLuint id);
	bool CheckCompileError(GLuint shaderID, SHADERTYPE type);

	GLuint m_iProgramID;
	GLuint m_iVertexShaderID = 0;
	GLuint m_iFragmentShaderID = 0;
};

