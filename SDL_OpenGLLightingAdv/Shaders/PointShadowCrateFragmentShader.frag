#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec4 fragPosLightSpace;
out vec4 finalColor;

uniform sampler2D sample;
uniform sampler2D shadowMap;
uniform vec3 eyePos;
uniform vec3 lightPos;
uniform float shininess;
bool isGamma;

//calculate shadow
float GetShadow(vec4 frag)
{
	vec3 fragCoords = frag.xyz / frag.w;
	fragCoords = fragCoords * 0.5f + 0.5f;
	float shadowMapDepth = texture(shadowMap, fragCoords.xy).r;
	float fragDepth = fragCoords.z;
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float bias = max(0.05f * (1.0 - dot(norm, lightDir)), 0.005f);

	float shadow = 0.0f;
	//PCF, use 9 samples
	vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			float pcfDepth = texture(shadowMap, fragCoords.xy + vec2(x, y) * texelSize).r;
			shadow += (fragDepth - bias) > pcfDepth ? 1.0f : 0.0f;
		}
	}

	shadow /= 9.0f;
	//shadow = (fragDepth - bias) > shadowMapDepth ? 1.0f : 0.0f;
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

	float shadow = GetShadow(fragPosLightSpace);
	vec3 effect = ambient + (1.0 - shadow) * (diffuse + specular);
	
	finalColor = vec4(effect, 1.0);
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
	BlinnPhongLighting();
	//NoTexture();
}

