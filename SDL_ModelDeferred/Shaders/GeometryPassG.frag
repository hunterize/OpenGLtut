#version 330

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpecular;

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D sample;
uniform bool hasTexture;

void main()
{
	gPosition = fragPos;
	gNormal = normalize(normal);

	if(hasTexture)
		gAlbedoSpecular = texture(sample, texCoord);
	else
		gAlbedoSpecular = vec4(0.75, 0.95, 0.75, 1.0);
}