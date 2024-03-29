#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
 
 //light position in world space
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform vec3 lightColor;
uniform float shininess;
uniform float linear;
uniform float quadratic;

//calculate lighting in world space
void Lighting()
{
	vec3 position = texture(gPosition, texCoord).rgb;

	vec3 normal = texture(gNormal, texCoord).rgb;
	vec3 albedo = texture(gAlbedo, texCoord).rgb;
	vec3 specu = vec3(texture(gAlbedo, texCoord).a);

	//direction vectors for lighting
	vec3 viewDir = normalize(eyePos - position);
	vec3 lightDir = normalize(lightPos - position);
	vec3 halfVector = normalize(viewDir + lightDir);

	//ambient
	vec3 ambient = 0.25 * vec3(albedo);

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

	vec3 effect = ambient + (diffuse + specular) * lightColor;

	color = vec4(effect, 1.0);
	//color = vec4(specu, 1.0);

}

void Debug(int id)
{
	vec3 position = normalize(texture(gPosition, texCoord).rgb);
	position = (position + 1.0) / 2.0f;
	vec3 normal = normalize(texture(gNormal, texCoord).rgb);
	normal = (normal + 1.0) / 2.0f;
	vec3 albedo = texture(gAlbedo, texCoord).rgb;

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
		default:
		break;
	}
}

void main()
{
	Lighting();
	//Debug(1);
}

