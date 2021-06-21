#version 330

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpecular;

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

void main()
{
	gPosition = fragPos;
	gNormal = normalize(normal);
	gAlbedoSpecular.xyz = vec3(0.9);
	gAlbedoSpecular.w = 1.0;
}