#version 330

layout (location = 0) in vec3 vertexPos;

uniform mat4 model;

void main()
{
	gl_Position = model * vec4(vertexPos, 1.0);
}
