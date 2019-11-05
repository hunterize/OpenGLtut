#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 finalColor;

uniform sampler2D sample;
uniform vec3 lightPosition[3];
uniform vec3 lightColor[3];
uniform vec3 eyePos;
uniform float shininess;
uniform bool isGamma;

void BlinnPhongWithGammaCorrection()
{
	vec3 lightPos = vec3(-10.0, 5.0, 0.0);

	float distance = length(lightPos - fragPos);
	float attenuation = 1.0f / (distance);

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(sample, texCoord));
	
	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(0.6);

	float ambi = 0.01f;
	vec3 ambient = ambi * vec3(texture(sample, texCoord));

	vec3 effect = ambient + diffuse + specular;
	
	float gc = 2.2f;
	finalColor = vec4(pow(effect, vec3(1.0 / gc)), 1.0);
}

void NoTexture()
{
	vec3 objColor = vec3(0.5, 0.0, 0.0);

	if(isGamma)
	{
		float g = 2.2f;
		finalColor = vec4(pow(objColor.rgb, vec3(1.0 / g)), 1.0);
	}
	else
		finalColor = vec4(objColor, 1.0);	
}

void MultipleLighting()
{
	float ambi = 0.06f;
	vec3 textureColor = vec3(texture(sample, texCoord));
	vec3 ambient = ambi * textureColor;
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(eyePos - fragPos);
	
	for(int i = 0; i < 3; i++)
	{
		float distance = length(lightPosition[i] - fragPos);
		float attenuation = 1.0f / (distance);

		vec3 lightDir = normalize(lightPosition[i] - fragPos);

		float diff = max(dot(norm, lightDir), 0.0);
		diffuse += diff * textureColor * lightColor[i] * attenuation;

		vec3 halfVector = normalize(lightDir + viewDir);
		float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
		specular += spec * vec3(0.7) * lightColor[i] * attenuation;
	}

	vec3 effect = ambient + diffuse + specular;

	float gc = 2.2f;
	finalColor = vec4(pow(effect, vec3(1.0 / gc)), 1.0);
}

void main()
{
	//NoTexture();
	//BlinnPhongWithGammaCorrection();
	MultipleLighting();
}

