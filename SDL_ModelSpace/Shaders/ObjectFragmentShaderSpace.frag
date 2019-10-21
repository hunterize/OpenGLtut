#version 330

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

out vec4 color;

layout (std140) uniform Lights
{
	vec4 lightPos;
	vec4 lightColor;
	vec4 eyePos;
	float shininess;
};

uniform sampler2D texture_ambient;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_emission;

uniform int eff;

void main()
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(vec3(lightPos) - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(texture_diffuse, texCoord));

	vec3 viewDir = normalize(vec3(eyePos) - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = spec * vec3(lightColor) * vec3(texture(texture_specular, texCoord));

	vec3 ambient = 0.2f * vec3(texture(texture_ambient, texCoord));

	vec3 effect = ambient + diffuse + specular;

	color = vec4(effect, 1.0);
}