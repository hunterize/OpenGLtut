#version 330

struct MATERIAL
{	
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};


struct DIRLIGHT
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct POINTLIGHT
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct SPOTLIGHT
{
	vec3 direction;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
	float innercutoff;
	float outercutoff;
};

#define PLNUM 3

uniform MATERIAL material;
uniform vec3 eyePos;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
out vec4 color;

uniform DIRLIGHT dirLight;
uniform POINTLIGHT pointLight[PLNUM];
uniform SPOTLIGHT spotLight;

vec3 GetDirectionalLight(DIRLIGHT light, vec3 normal, vec3 fragmentPosition, vec3 eyePosition);
vec3 GetPointLight(POINTLIGHT light, vec3 normal, vec3 fragmentPosition, vec3 eyePosition);
vec3 GetSpotLight(SPOTLIGHT light, vec3 normal, vec3 fragmentPosition, vec3 eyePosition);

void main()
{
	vec3 effect;
	//get directional light
	effect = GetDirectionalLight(dirLight, normal, fragPos, eyePos);
	//get point light

	for(int i = 0; i < PLNUM; i++)
	{
		effect += GetPointLight(pointLight[i], normal, fragPos, eyePos);
	}

	//get spot light
	effect += GetSpotLight(spotLight, normal, fragPos, eyePos);

	vec3 emission = vec3(texture(material.emission, texCoord));
	effect += emission;

	color = vec4(effect, 1.0);
}

vec3 GetDirectionalLight(DIRLIGHT light, vec3 normal, vec3 fragmentPosition, vec3 eyePosition)
{
	//calculate ambient of the light
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

	//calculate diffuse of the light
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));

	//calculate specular of the light
	vec3 viewDir = normalize(eyePosition - fragmentPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));

	return ambient + diffuse + specular;

}

vec3 GetPointLight(POINTLIGHT light, vec3 normal, vec3 fragmentPosition, vec3 eyePosition)
{
	vec3 lightDir = normalize(light.position - fragmentPosition);
	float distance = length(light.position - fragmentPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	
	//calculate ambient of the light
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

	//calculate diffuse of the light
	vec3 norm = normalize(normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));

	//calculate specular of the light
	vec3 viewDir = normalize(eyePosition - fragmentPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));

	return attenuation * (ambient + diffuse + specular);
}

vec3 GetSpotLight(SPOTLIGHT light, vec3 normal, vec3 fragmentPosition, vec3 eyePosition)
{
	vec3 lightDir = normalize(light.position - fragmentPosition);
	float distance = length(light.position - fragmentPosition);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	
	//calculate ambient of the light
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

	//calculate diffuse of the light
	vec3 norm = normalize(normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));

	//calculate specular of the light
	vec3 viewDir = normalize(eyePosition - fragmentPosition);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));

	float lightAngle = dot(lightDir, normalize(-light.direction));
	float flashlightIntensity = clamp((lightAngle - light.outercutoff) / (light.innercutoff - light.outercutoff), 0.0, 1.0);

	diffuse *= flashlightIntensity;
	specular *= flashlightIntensity;

	return attenuation * (ambient + diffuse + specular);
}