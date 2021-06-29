#version 330

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpecular;

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

void main()
{
	gPosition = fragPos;
	gNormal = normalize(normal);
	gAlbedoSpecular.xyz = vec3(texture(texture_diffuse1, texCoord));
	gAlbedoSpecular.w = texture(texture_specular1, texCoord).r;
}