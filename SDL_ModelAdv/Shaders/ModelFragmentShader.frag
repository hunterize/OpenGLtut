#version 330

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

uniform vec3 lightPos;
uniform vec3 eyePos;
uniform float shininess;

void main()
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(texture_diffuse1, texCoord));

	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = spec * vec3(texture(texture_specular1, texCoord));

	vec3 ambient = 0.5f * vec3(texture(texture_diffuse1, texCoord));

	vec3 effect = ambient + diffuse + specular;

	color = vec4(effect, 1.0);
}