#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 color;

uniform sampler2D sample;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform float shininess;

uniform bool isBlinn;

void main()
{
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(sample, texCoord));
	
	vec3 viewDir = normalize(eyePos - fragPos);
	float spec = 0.0f;

	if(isBlinn)
	{
		vec3 halfVector = normalize(lightDir + viewDir);
		spec = pow(max(dot(norm, halfVector), 0.0), shininess);
	}
	else
	{		
		vec3 reflectDir = reflect(-lightDir, norm);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	}

	vec3 specular = spec * vec3(0.6);

	float ambi = 0.01f;
	vec3 ambient = ambi * vec3(texture(sample, texCoord));

	vec3 effect = ambient + diffuse + specular;
	
	color = vec4(effect, 1.0);
}

