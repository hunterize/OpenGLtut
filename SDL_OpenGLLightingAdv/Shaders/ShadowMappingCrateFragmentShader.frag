#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 finalColor;

uniform sampler2D sample;
uniform vec3 eyePos;
uniform float shininess;
uniform bool isGamma;

void BlinnPhongLighting()
{
	vec3 lightPos = vec3(-100.0, 50.0, 0.0);

	float distance = length(lightPos - fragPos);

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(sample, texCoord));
	
	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(0.6);

	float ambi = 0.1f;
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

void main()
{
	//BlinnPhongLighting();
	NoTexture();
}

