#version 330

struct MATERIAL
{	
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct LIGHT
{
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	//for spot light
	float constant;
	float linear;
	float quadratic;
	//for flashlight
	float cutoff;
	float outercutoff;
	
};

uniform MATERIAL material;
uniform LIGHT light;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 eysPos;


in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
out vec4 color;

void main()
{
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

	vec3 norm = normalize(normal);
	
	//for point light
	vec3 lightDir = normalize(light.position - fragPos);
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	
	//for direction light
	//vec3 lightDir = -light.direction;
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));

	vec3 viewDir = normalize(eysPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));

	vec3 emission = vec3(texture(material.emission, texCoord));

	//use direction light
	//vec3 effect = ambient + diffuse + specular + emission;

	//use attenuation point light 
	//vec3 effect = attenuation * (ambient + diffuse + specular + emission);

	//use flash light, cos value of the angle
	float lightAngle = dot(lightDir, normalize(-light.direction));
	float flashlightIntensity = clamp((lightAngle - light.outercutoff) / (light.cutoff - light.outercutoff), 0.0, 1.0);

	diffuse *= flashlightIntensity;
	specular *= flashlightIntensity;
	
	vec3 effect = attenuation * (ambient + diffuse + specular + emission);

	color = vec4(effect, 1.0f);
}