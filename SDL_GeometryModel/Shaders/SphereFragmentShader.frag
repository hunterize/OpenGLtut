#version 330

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;
in mat3 TBN;

uniform vec3 eyePos;
uniform vec3 lightPos;
uniform float shininess;
uniform float heightScale;

out vec4 color;

uniform sampler2D amap;
uniform sampler2D bmap;
uniform sampler2D smap;

void BlinnPhongLighting()
{
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(amap, texCoord));

	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(texture(smap, texCoord)) * vec3(1.0);

	float ambi = 0.1f;
	vec3 ambient = ambi * vec3(texture(amap, texCoord));

	color = vec4(diffuse + specular + ambient, 1.0);
}

//get parallax mapping texture coordinate with depth map
vec2 ParallaxOcclusionMappingTextureDepth(vec2 textureCoord, vec3 viewDir)
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
	float currentDepthValue = texture(bmap, currentTexCoord).r;


	while(currentLayerDepth < currentDepthValue)
	{
		currentTexCoord -= deltaV;
		currentDepthValue = texture(bmap, currentTexCoord).r;
		currentLayerDepth += layerDepth;
	}

	vec2 preTexCoord = currentTexCoord + deltaV;
	float preLayerDepth = currentLayerDepth - layerDepth;

	float currentOffset = currentLayerDepth - currentDepthValue;
	float previousOffset = texture(bmap, preTexCoord).r - preLayerDepth;

	float weight = currentOffset / (currentOffset + previousOffset);

	vec2 finalTexCoord = preTexCoord * weight + currentTexCoord * (1 - weight);

	return finalTexCoord;

}

//get parallax mapping texture coordinate with height map
vec2 ParallaxOcclusionMappingTextureHeight(vec2 textureCoord, vec3 viewDir)
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
	float currentDepthValue = 1.0 - texture(bmap, currentTexCoord).r;


	while(currentLayerDepth < currentDepthValue)
	{
		currentTexCoord -= deltaV;
		currentDepthValue = 1.0 - texture(bmap, currentTexCoord).r;
		currentLayerDepth += layerDepth;
	}

	vec2 preTexCoord = currentTexCoord + deltaV;
	float preLayerDepth = currentLayerDepth - layerDepth;

	float currentOffset = currentLayerDepth - currentDepthValue;
	float previousOffset = (1.0 - texture(bmap, preTexCoord).r) - preLayerDepth;

	float weight = currentOffset / (currentOffset + previousOffset);

	vec2 finalTexCoord = preTexCoord * weight + currentTexCoord * (1 - weight);

	return finalTexCoord;

}

//in tangent space, without normal map
void ParallaxMapping()
{
	vec3 eyePos_t = TBN * eyePos;
	vec3 fragPos_t = TBN * fragPos;
	vec3 lightPos_t = TBN * lightPos;
	vec3 normal_t = TBN * normal;

	vec3 viewDir = normalize(eyePos_t - fragPos_t);
	vec2 mappedTexCoord = ParallaxOcclusionMappingTextureHeight(texCoord, viewDir);

	if(mappedTexCoord.x > 1.0 || mappedTexCoord.y > 1.0
	|| mappedTexCoord.x < 0.0 || mappedTexCoord.y < 0.0)
	{
		discard;
	}
	
	vec3 lightDir = normalize(lightPos_t - fragPos_t);
	float diff = max(dot(normal_t, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(amap, mappedTexCoord));


	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal_t, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(texture(smap, mappedTexCoord)) * vec3(1.0);

	float ambi = 0.1f;
	vec3 ambient = ambi * vec3(texture(amap, mappedTexCoord));

	color = vec4(diffuse + specular + ambient, 1.0);
	//color = vec4(vec3(mappedTexCoord.x), 1.0);
}

//in tangent space with normal map
void ParallaxMapping_NormalMap()
{
	vec3 eyePos_t = TBN * eyePos;
	vec3 fragPos_t = TBN * fragPos;
	vec3 lightPos_t = TBN * lightPos;
	//vec3 normal_t = TBN * normal;

	vec3 viewDir = normalize(eyePos_t - fragPos_t);
	vec2 mappedTexCoord = ParallaxOcclusionMappingTextureDepth(texCoord, viewDir);

	if(mappedTexCoord.x > 1.0 || mappedTexCoord.y > 1.0
	|| mappedTexCoord.x < 0.0 || mappedTexCoord.y < 0.0)
	{
		discard;
	}

	vec3 normal_t = texture(smap, mappedTexCoord).rgb;
	normal_t = normalize(normal_t * 2.0 - 1.0);
	
	vec3 lightDir = normalize(lightPos_t - fragPos_t);
	float diff = max(dot(normal_t, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(amap, mappedTexCoord));


	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal_t, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(1.0);

	float ambi = 0.1f;
	vec3 ambient = ambi * vec3(texture(amap, mappedTexCoord));

	color = vec4(diffuse + specular + ambient, 1.0);
	//color = vec4(vec3(mappedTexCoord.x), 1.0);
}

void main()
{
	BlinnPhongLighting();
	//ParallaxMapping();
	//ParallaxMapping_NormalMap();
}

