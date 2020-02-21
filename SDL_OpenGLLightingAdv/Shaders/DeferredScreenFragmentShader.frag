#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpecular;

void Debug()
{
	vec3 position = normalize(texture(gPosition, texCoord).rgb);
	position = (position + vec3(1.0)) / 2.0f;
	vec3 normal = normalize(texture(gNormal, texCoord).rgb);
	normal = (normal + vec3(1.0)) / 2.0f;
	vec3 albedo = texture(gAlbedoSpecular, texCoord).rgb;
	
	color = vec4(position, 1.0);
}

void main()
{
	Debug();
}



