#version 330

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

out vec4 color;

uniform sampler2D texture_ambient1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform samplerCube env;


uniform vec3 lightPos;
uniform vec3 eyePos;
uniform float shininess;

void soldier()
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(texture_diffuse1, texCoord));

	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = spec * vec3(texture(texture_specular1, texCoord));

	vec3 ambient = 0.8f * vec3(texture(texture_diffuse1, texCoord));

	vec3 effect = ambient + diffuse + specular;

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

	vec3 effect = mapping * ambient + diffuse + specular;

	color = vec4(effect, 1.0);
}

void chrome()
{
	vec3 lookat = normalize(fragPos - eyePos);
	vec3 reflection = reflect(lookat, normalize(normal));
	vec4 texColor = texture(env, reflection);
	color = vec4(texColor.rgb, 1.0);
}

void invisible()
{
	float ratio = 1.0 / 1.3;
	vec3 lookat = normalize(fragPos - eyePos);
	vec3 refraction = refract(lookat, normalize(normal), ratio);
	vec4 texColor = texture(env, refraction);
	color = vec4(texColor.rgb, 1.0);
}

void main()
{
	//soldier();
	//chrome();
	//invisible();
	soldierenv();
}