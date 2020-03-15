#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNor;
layout (location = 2) in vec3 vertexTan;
layout (location = 3) in vec2 vertexUV;

uniform mat4 model;
uniform	mat4 projection;
uniform	mat4 view;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}
