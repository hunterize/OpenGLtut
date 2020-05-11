#version 330

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

in mat3 TBN;

out vec4 color;

uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform samplerCube env;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 eyePos;
uniform float shininess;

uniform int col;

void soldier()
{
	vec3 norm = normalize(normal);

	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(texture_diffuse1, texCoord));

	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(texture(texture_specular1, texCoord));

	vec3 ambient = 0.7f * vec3(texture(texture_diffuse1, texCoord));

	vec3 effect = lightColor * (ambient + diffuse + specular);

	color = vec4(effect, 1.0);
}

void grey()
{
	vec3 norm = normalize(normal);

	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(0.5);

	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(0.5);

	vec3 ambient = 0.7f * vec3(0.5);

	vec3 effect = lightColor * (ambient + diffuse + specular);

	color = vec4(effect, 1.0);
}

void main()
{
	switch(col)
	{
		case 1:
			soldier();
			break;
		case 2:
			grey();
			break;
		case 3:
			color = vec4(0.5, 0.5, 0.5, 1.0);
			break;
		default:
			soldier();
	}
}