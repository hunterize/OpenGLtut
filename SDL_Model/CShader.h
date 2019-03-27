#pragma once

#include <string>
#include <glew.h>

#include <fstream>
#include <sstream>
#include <iostream>

#include <glm.hpp>

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

	void SetUniformInt(const std::string& name, int value) const;
	void SetUniformFloat(const std::string& name, float value) const;
	void SetUniformVec2(const std::string& name, const glm::vec2& value) const;
	void SetUniformVec3(const std::string& name, const glm::vec3& value) const;
	void SetUniformVec4(const std::string& name, const glm::vec4& value) const;
	void SetUniformMat4(const std::string& name, const glm::mat4& value) const;

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

