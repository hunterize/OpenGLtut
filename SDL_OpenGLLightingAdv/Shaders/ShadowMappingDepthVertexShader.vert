#version 330

layout (location = 0) in vec3 vertexPos;

uniform mat4 model;
uniform	mat4 projection;
uniform	mat4 view;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}
