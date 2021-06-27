#version 330

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpecular;

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

//wall object is not from model
void main()
{
	gPosition = fragPos;
	gNormal = normalize(normal);
	gAlbedoSpecular = vec4(0.95, 0.75, 0.75, 1.0);
}