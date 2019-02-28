#version 330

struct MATERIAL
{	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct LIGHT
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform MATERIAL material;
uniform LIGHT light;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 eysPos;


in vec3 normal;
in vec3 fragPos;
out vec4 color;

void main()
{
	vec3 ambient = light.ambient * material.ambient;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * material.diffuse;

	vec3 viewDir = normalize(eysPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * material.specular;

	vec3 effect = ambient + diffuse + specular;
	color = vec4(effect, 1.0f);
}