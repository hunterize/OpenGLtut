#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D aoTexture;
 
 //light position in view space
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float shininess;
uniform float linear;
uniform float quadratic;

//calculate lighting in view space
void Lighting()
{
	vec3 position = texture(gPosition, texCoord).rgb;

	vec3 normal = texture(gNormal, texCoord).rgb;
	//vec3 albedo = texture(gAlbedo, texCoord).rgb;
	vec3 albedo = vec3(0.85);
	vec3 specu = vec3(texture(gAlbedo, texCoord).a);

	//eye position is 0.0 in view space
	vec3 viewDir = normalize(-position);
	vec3 lightDir = normalize(lightPos - position);
	vec3 halfVector = normalize(viewDir + lightDir);

	//ambient
	vec3 ambient = 0.325 * vec3(albedo);

	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * albedo;

	//specular
	float spec = 0.0f;
	if(dot(normal, viewDir) > 0.0)
	{
		spec = pow(max(dot(normal, halfVector), 0.0), shininess);
	}
	vec3 specular = spec * specu;

	//attenuation
	float distance = length(lightPos - position);
	float attenuation = 1.0 / (1.0 + linear * distance + quadratic * distance * distance);

	vec3 effect = ambient + attenuation * (diffuse + specular) * lightColor;

	color = vec4(effect, 1.0);
}

void Debug(int id)
{
	vec3 position = normalize(texture(gPosition, texCoord).rgb);
	position = (position + 1.0) / 2.0f;
	vec3 normal = normalize(texture(gNormal, texCoord).rgb);
	normal = (normal + 1.0) / 2.0f;
	vec3 albedo = texture(gAlbedo, texCoord).rgb;
	vec3 ao = vec3(texture(aoTexture, texCoord).r);

	switch(id)
	{
		case 0:
		color = vec4(position, 1.0);
		break;
		case 1:
		color = vec4(normal, 1.0);
		break;
		case 2:
		color = vec4(albedo, 1.0);
		break;
		case 3:
		color = vec4(ao, 1.0);
		default:
		break;
	}
}

void main()
{
	//Lighting();
	Debug(3);
}

