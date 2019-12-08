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
uniform int effect;

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

	vec3 ambient = 0.2f * vec3(texture(texture_diffuse1, texCoord));

	vec3 effect = lightColor * (ambient + diffuse + specular);

	color = vec4(effect, 1.0);
}

void DebugNormal()
{
	vec3 normal = vec3(texture(texture_normal1, texCoord));
	normal = normalize(normal * 2.0 -1.0);
	normal = normalize(TBN * normal);
	color = vec4(normal, 1.0);
}

void soldierNormal()
{
	//vec3 norm = normalize(normal);
	vec3 norm = texture(texture_normal1, texCoord).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	norm = normalize(TBN * norm);

	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(texture_diffuse1, texCoord));

	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(texture(texture_specular1, texCoord));

	vec3 ambient = 0.2f * vec3(texture(texture_diffuse1, texCoord));

	vec3 effect = lightColor * (ambient + diffuse + specular);

	color = vec4(effect, 1.0);
}

void soldierenv()
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(texture_diffuse1, texCoord));

	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = spec * vec3(texture(texture_specular1, texCoord));

	vec3 ambient = 0.6 * vec3(texture(texture_ambient1, texCoord));

	vec3 lookat = normalize(fragPos - eyePos);
	vec3 reflection = reflect(lookat, normalize(normal));
	vec3 mapping = texture(env, reflection).rgb;

	vec3 effect = lightColor * (mapping * ambient + diffuse + specular);

	color = vec4(effect, 1.0);
}

void main()
{
	switch(effect)
	{
		case 0:
			DebugNormal();
			break;
		case 1:
			soldier();
			break;
		case 2:
			soldierNormal();
			break;
		default:
			soldier();
	}
}