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

	void AttachShader(const char* vertexShaderFilePath = NULL, const char* fragmentShaderPath = NULL, const char* geometryShaderFilePath = NULL);

	void SetUniformInt(const std::string& name, int value) const;
	void SetUniformFloat(const std::string& name, float value) const;
	void SetUniformVec2(const std::string& name, const glm::vec2& value) const;
	void SetUniformVec3(const std::string& name, const glm::vec3& value) const;
	void SetUniformVec4(const std::string& name, const glm::vec4& value) const;
	void SetUniformMat4(const std::string& name, const glm::mat4& value) const;

	void SetUniformVec2Array(const std::string& name, const glm::vec2 value[], int size) const;
	void SetUniformVec3Array(const std::string& name, const glm::vec3 value[], int size) const;
	void SetUniformVec4Array(const std::string& name, const glm::vec4 value[], int size) const;
	void SetUniformMat4Array(const std::string& name, const glm::mat4 value[], int size) const;

	GLuint GetID();

	void Use();
	void Unuse();

	void Dispose();

private:
	bool CompileShader(const char* filePath, GLuint id);
	bool LinkShader();
	bool CheckCompileError(GLuint shaderID);
	bool CheckLinkError(GLuint shaderID);

	GLuint m_iProgramID = 0;
	GLuint m_iVertexShaderID = 0;
	GLuint m_iFragmentShaderID = 0;
	GLuint m_iGeometryShaderID = 0;
};

