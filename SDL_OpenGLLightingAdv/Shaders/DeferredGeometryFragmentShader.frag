#version 330

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpecular;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

in mat3 TBN;

uniform sampler2D sample;
uniform sampler2D nmap;

void GeometryBuffers()
{
	//position buffer
	gPosition = fragPos;

	//normal buffer
	vec3 norm = texture(nmap, texCoord).rgb;
	norm = normalize(norm * 2.0 -1.0);
	norm = normalize(TBN * norm);
	gNormal = norm;

	//color buffer and specular intensity value
	gAlbedoSpecular.rgb = texture(sample, texCoord).rgb;
	gAlbedoSpecular.a = 1.0;
	
}

void main()
{
	GeometryBuffers();
}

