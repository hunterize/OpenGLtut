#version 330

layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec2 texCoords;

out vec2 texCoord;

void main()
{
	gl_Position = vec4(vertexPos, 0.0, 1.0);
	texCoord = texCoords;
}
