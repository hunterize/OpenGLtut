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

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 ambient = vec3(0.0);

	for(int i = 0; i < 4; i++)
	{
		float distance = length(lights[i].position - fragPos);
		float attenuation = 1.0 / (1.0 + 0.02 * distance + 0.001 * distance * distance);

		vec3 norm = normalize(normal);

		vec3 lightDir = normalize(lights[i].position - fragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse += attenuation * diff * vec3(texture(sample, texCoord)) * lights[i].color;
	
		if(dot(norm, lightDir) > 0.0)
		{
			vec3 viewDir = normalize(eyePos - fragPos);
			vec3 halfVector = normalize(lightDir + viewDir);
			float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
			specular += attenuation * spec * vec3(0.6) * lights[i].color;
		}

		float ambi = 0.1f;
		ambient += attenuation * ambi * vec3(texture(sample, texCoord)) * lights[i].color;
	}

	vec3 effect = ambient + diffuse + specular;

	float brightness = dot(effect.rgb, vec3(0.2126, 0.7152, 0.1722));
	if(brightness > 10.0)
	{
		brightColor = vec4(effect.rgb, 1.0);
	}
	else
	{
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}

	finalColor = vec4(effect, 1.0);
}

void main()
{
	BlinnPhongLighting();
}

