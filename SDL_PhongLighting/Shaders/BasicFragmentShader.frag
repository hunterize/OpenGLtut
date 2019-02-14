#version 330

uniform vec3 fragmentColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 eysPos;
uniform float ambientStrength;
uniform float specularStrength;

in vec3 normal;
in vec3 fragPos;
out vec4 color;

void main()
{
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 viewDir = normalize(eysPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 effect = (ambient + diffuse + specular) * fragmentColor;
	color = vec4(effect, 1.0f);
}