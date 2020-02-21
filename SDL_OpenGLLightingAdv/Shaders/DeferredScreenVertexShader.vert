#version 330

layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec2 vertexUV;

out vec2 texCoord;

void main()
{
	gl_Position = vec4(vertexPos.x, vertexPos.y, 0.0, 1.0);
	texCoord = vertexUV;
}