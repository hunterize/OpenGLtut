#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

in mat3 TBN;

out vec4 finalColor;

uniform sampler2D sample;
uniform sampler2D nmap;
uniform sampler2D depthmap;

uniform vec3 eyePos;
uniform vec3 lightPos;
uniform float shininess;

uniform bool isNormalReverse;
uniform float heightScale;

uniform bool isPOM;

//Blinn-Phong lighting with normal mapping in world space
void NormalMapping()
{	
	//vec3 norm = normalize(normal);
	vec3 norm = texture(nmap, texCoord).rgb;
	norm = normalize(norm * 2.0 - 1.0);

	norm = normalize(TBN * norm);

	if(isNormalReverse)
	{
		norm = vec3(-1.0) * norm;
	}
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(sample, texCoord));
	
	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(0.3);

	float ambi = 0.1f;
	vec3 ambient = ambi * vec3(texture(sample, texCoord));

	vec3 effect = ambient + diffuse + specular;
	
	finalColor = vec4(effect, 1.0);
}

vec2 ParallaxMappingTexture(vec2 textureCoord, vec3 viewDir)
{
	float h = texture(depthmap, textureCoord).r;
	//return textureCoord - (viewDir.xy * h / viewDir.z) * heightScale;
	return textureCoord - viewDir.xy * h * heightScale;
}

vec2 ParallaxMappingTextureEx(vec2 textureCoord, vec3 viewDir)
{
	int num = 10;
	float layerDepth = 1.0 / num;
	float currentLayerDepth = 0.0;

	vec2 Vxy = viewDir.xy * heightScale;
	vec2 deltaV = Vxy / num;

	vec2 currentTexCoord = textureCoord;
	float currentDepthValue = texture(depthmap, currentTexCoord).r;

	while(currentLayerDepth < currentDepthValue)
	{
		currentTexCoord -= deltaV;
		currentDepthValue = texture(depthmap, currentTexCoord).r;
		currentLayerDepth += layerDepth;
	}

	return currentTexCoord;

}

vec2 ParallaxOcclusionMappingTexture(vec2 textureCoord, vec3 viewDir)
{
	//int num = 10;

	float minLayer = 8;
	float maxLayer = 32;
	float num = mix(maxLayer, minLayer, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	float layerDepth = 1.0 / num;
	float currentLayerDepth = 0.0;

	vec2 Vxy = viewDir.xy * heightScale;
	vec2 deltaV = Vxy / num;

	vec2 currentTexCoord = textureCoord;
	float currentDepthValue = texture(depthmap, currentTexCoord).r;

	while(currentLayerDepth < currentDepthValue)
	{
		currentTexCoord -= deltaV;
		currentDepthValue = texture(depthmap, currentTexCoord).r;
		currentLayerDepth += layerDepth;
	}

	vec2 preTexCoord = currentTexCoord + deltaV;
	float preLayerDepth = currentLayerDepth - layerDepth;

	//float afterDepth = currentDepthValue - currentLayerDepth;
	//float beforeDepth = texture(depthmap, preTexCoord).r - preLayerDepth;
	//float weight = afterDepth / (afterDepth - beforeDepth);

	float currentOffset = currentLayerDepth - currentDepthValue;
	float previousOffset = texture(depthmap, preTexCoord).r - preLayerDepth;
	float weight = currentOffset / (currentOffset + previousOffset);

	vec2 finalTexCoord = preTexCoord * weight + currentTexCoord * (1 - weight);

	return finalTexCoord;

}

//calculate lighting in tangent space
void ParallaxMapping()
{	
	vec3 tangentViewPos = TBN * eyePos;
	vec3 tangentFragPos = TBN * fragPos;
	vec3 tangentLightPos = TBN * lightPos;

	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
	vec2 mappedTexCoord = ParallaxOcclusionMappingTexture(texCoord, viewDir);

	if(mappedTexCoord.x > 1.0 || mappedTexCoord.y > 1.0
	|| mappedTexCoord.x < 0.0 || mappedTexCoord.y < 0.0)
	{
		discard;
	}
	
	vec3 norm = texture(nmap, mappedTexCoord).rgb;
	norm = normalize(norm * 2.0 - 1.0);

	if(isNormalReverse)
	{
		norm = vec3(-1.0) * norm;
	}

	vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(sample, mappedTexCoord));
	
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(0.3);

	float ambi = 0.1f;
	vec3 ambient = ambi * vec3(texture(sample, mappedTexCoord));

	vec3 effect = ambient + diffuse + specular;
	
	finalColor = vec4(effect, 1.0);
}


void main()
{
	if(isPOM)
	{
		ParallaxMapping();
	}
	else
	{
		NormalMapping();
	}
}

