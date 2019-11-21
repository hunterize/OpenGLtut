#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 finalColor;

uniform sampler2D sample;
uniform samplerCube shadowMap;

uniform vec3 eyePos;
uniform vec3 lightPos;
uniform float shininess;

uniform float far_plane;
uniform bool isDebug;

//calculate shadow
float GetShadow(vec3 fragPos)
{
	//get depth value from cube map
	vec3 lightLookat = fragPos - lightPos;
	
	float storedDepth = texture(shadowMap, lightLookat).r;
	storedDepth *= far_plane;
	
	float fragDepth = length(lightLookat);

	float bias = 0.05f;
	float shadow = fragDepth - bias > storedDepth ? 1.0f : 0.0f;

	return shadow;

}

//Blinn-Phong lighting
void BlinnPhongLighting()
{
	float distance = length(lightPos - fragPos);

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(sample, texCoord));
	
	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(0.6);

	float ambi = 0.3f;
	vec3 ambient = ambi * vec3(texture(sample, texCoord));

	float shadow = GetShadow(fragPos);
	vec3 effect = ambient + (1.0 - shadow) * (diffuse + specular);
	
	finalColor = vec4(effect, 1.0);
}

void DebugShadow()
{
	//get depth value from cube map
	vec3 lightLookat = fragPos - lightPos;
	
	float storedDepth = texture(shadowMap, lightLookat).r;

	finalColor = vec4(vec3(storedDepth), 1.0);
}

void main()
{
	if(isDebug)
	{
		DebugShadow();
	}
	else
	{
		BlinnPhongLighting();
	}

}

