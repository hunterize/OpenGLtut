#version 330

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

out vec4 color;

layout (std140) uniform plaLights
{
	vec4 lightPos;
	vec4 lightColor;
	vec4 eyePos;
	float shininess;
};

uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform samplerCube env;

uniform int eff;

void planet()
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(vec3(lightPos) - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(texture_diffuse1, texCoord));

	vec3 viewDir = normalize(vec3(eyePos) - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = spec * vec3(texture(texture_specular1, texCoord));

	vec3 ambient = 0.8f * vec3(texture(texture_diffuse1, texCoord));

	vec3 effect = vec3(lightColor) * (ambient + diffuse + specular);

	color = vec4(effect, 1.0);
}

void planetenv()
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(vec3(lightPos) - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(texture_diffuse1, texCoord));

	vec3 viewDir = normalize(vec3(eyePos) - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = spec * vec3(texture(texture_specular1, texCoord));

	vec3 ambient = 0.6f * vec3(texture(texture_ambient1, texCoord));

	vec3 lookat = normalize(fragPos - vec3(eyePos));
	vec3 reflection = reflect(lookat, normalize(normal));
	//vec3 mapping = texture(env, reflection).rgb;
	vec3 mapping = vec3(0.1, 0.1, 0.1);

	vec3 effect = vec3(lightColor) * (mapping * ambient + diffuse + specular);

	color = vec4(effect, 1.0);
}

void main()
{

	switch(eff)
	{
		case 1:
			planet();
			break;
		case 2:
			planetenv();
			break;
		default:
			color = texture(texture_ambient1, texCoord);
			break;
	}

}