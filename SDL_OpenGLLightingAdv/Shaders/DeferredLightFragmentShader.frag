#version 330

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpecular;

uniform vec3 lightColor;

void main()
{
	gPosition = vec3(1.0);
	gNormal = vec3(1.0f);
	gAlbedoSpecular.rgb = lightColor;
}