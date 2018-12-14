#version 330

layout (location = 0) in vec2 vertexPos;
layout (location = 1) in vec3 vertexColor;
out vec3 fragmentColor;
void main()
{
	gl_Position = vec4(vertexPos.x, vertexPos.y, 0, 1.0);
	fragmentColor = vertexColor;
}