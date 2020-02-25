#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpecular;

uniform vec3 eyePos;
uniform float shininess;

struct Light
{
	vec3 position;
	vec3 color;
	float radius;
};

uniform Light lights[1331];

uniform int isDebug;
uniform int debugInfo;

void Debug(int id)
{
	vec3 position = normalize(texture(gPosition, texCoord).rgb);
	position = (position + 1.0) / 2.0f;
	vec3 normal = normalize(texture(gNormal, texCoord).rgb);
	normal = (normal + 1.0) / 2.0f;
	vec3 albedo = texture(gAlbedoSpecular, texCoord).rgb;
	
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

void BlinnPhongLightingMultiple()
{
	vec3 position = texture(gPosition, texCoord).rgb;
	vec3 normal = normalize(texture(gNormal, texCoord).rgb);
	vec3 albedo = texture(gAlbedoSpecular, texCoord).rgb;

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 ambient = vec3(0.0);

	vec3 viewDir = normalize(eyePos - position);

	for(int i = 600; i < 1200; i++)
	{
		float distance = length(lights[i].position - position);
		if(distance < lights[i].radius)
		{
			//float attenuation = 256.0 / (0.1f * (1.0f + lights[i].linear * distance + lights[i].quadratic * distance * distance));
			float attenuation = 1.0;
			vec3 lightDir = normalize(lights[i].position - position);
			float diff = max(dot(normal, lightDir), 0.0);
			diffuse +=  diff * albedo * lights[i].color * attenuation;
	
			if(dot(normal, lightDir) > 0.0)
			{
				vec3 halfVector = normalize(lightDir + viewDir);
				float spec = pow(max(dot(normal, halfVector), 0.0), shininess);
				specular +=  spec * vec3(1.0) * lights[i].color * attenuation;
			}
		}
	}
	float ambi = 0.5f;
	ambient +=  ambi * albedo;
	vec3 effect = ambient + diffuse + specular;
	color = vec4(effect, 1.0);
}

void main()
{
	//Debug(debugInfo);
	BlinnPhongLightingMultiple();

}



