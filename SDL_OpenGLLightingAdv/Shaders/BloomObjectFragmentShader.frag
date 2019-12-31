#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

layout (location = 0) out vec4 finalColor;
layout (location = 1) out vec4 brightColor;

uniform sampler2D sample;

uniform vec3 eyePos;
uniform vec3 lightPos;
uniform float shininess;

struct Light
{
	vec3 position;
	vec3 color;
};

uniform Light lights[4];

//Blinn-Phong lighting
void BlinnPhongLighting()
{	

	vec3 lighting = vec3(0.0);

	for(int i = 0; i < 4; i++)
	{
		float distance = length(lights[i].position - fragPos);
		float attenuation = 1.0 / (1.0 + 0.02 * distance + 0.001 * distance * distance);

		vec3 norm = normalize(normal);

		vec3 lightDir = normalize(lights[i].position - fragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * vec3(texture(sample, texCoord));
	
		vec3 viewDir = normalize(eyePos - fragPos);
		vec3 halfVector = normalize(lightDir + viewDir);
		float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
		vec3 specular = spec * vec3(0.5);

		float ambi = 0.1f;
		vec3 ambient = ambi * vec3(texture(sample, texCoord));

		vec3 effect = attenuation * (ambient + diffuse + specular) * lights[i].color;
		
		lighting += effect;
		//float gc = 2.2f;
		//effect = pow(effect, vec3(1.0 / gc));
	}

	finalColor = vec4(lighting, 1.0);
}

void HDRLighting()
{
	vec3 norm = normalize(normal);

	vec3 lighting = vec3(0.0);

	for(int i = 0; i < 4; i++)
	{
		vec3 lightDir = normalize(lights[i].position - fragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lights[i].color * vec3(texture(sample, texCoord));
		float distance = length(lights[i].position - fragPos);
		//float attenuation = 1.0 / (1.0 + 0.02 * distance + 0.001 * distance * distance);
		float attenuation = 1.0 / (distance * distance);
		lighting += diffuse * attenuation;
	}

	finalColor = vec4(lighting, 1.0);
}

void main()
{
	BlinnPhongLighting();
	//HDRLighting();

	float brightness = dot(finalColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0)
	{
		brightColor = vec4(finalColor.rgb, 1.0);
	}
	else
	{
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}

