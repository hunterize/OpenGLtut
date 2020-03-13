#version 330

in vec2 in_texCoord;
out vec4 out_color;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpecular;

uniform vec3 uLightPos;
uniform vec3 uEyePos;
uniform vec2 uScreenSize;
uniform float shininess;

struct CAttenuation
{
	float constant;
	float linear;
	float quadratic;
	float factor;
};

struct CLight
{
	vec3 position;
	vec3 color;
	CAttenuation attenuation;
};

uniform CLight uLight;

vec2 GetTexCoord()
{
	return gl_FragCoord.xy / uScreenSize;
}

vec3 BlinnPhongLighting(vec3 fragPos, vec3 lightPos, vec3 eyePos, vec3 normal, vec3 color)
{
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * color;

	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfVector), 0.0), shininess);
	vec3 specular = spec * color;

	float ambi = 0.1f;
	vec3 ambient = ambi * color;
	return ambient + diffuse + specular;
}

vec3 PointLighting(vec3 fragPos, vec3 eyePos, CLight pl, vec3 normal)
{
	vec3 distance = pl.position - fragPos;
	vec3 lightDir = normalize(distance);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * pl.color;

	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfVector), 0.0), shininess);
	vec3 specular = spec * pl.color;

	float ambi = 0.1f;
	vec3 ambient = ambi * pl.color;

	float d = length(distance);
	float attenuation = pl.attenuation.factor / (pl.attenuation.constant + pl.attenuation.linear * d + pl.attenuation.quadratic * d * d);

	return (ambient + diffuse + specular) * attenuation;
}

void main()
{
	vec2 texCoord = GetTexCoord();

	vec3 position = texture(gPosition, texCoord).xyz;
	vec3 normal = normalize(texture(gNormal, texCoord).xyz);
	vec3 fragColor = texture(gAlbedoSpecular, texCoord).xyz;
	
	//vec3 effect = BlinnPhongLighting(position, uLightPos, uEyePos, normal, fragColor);

	vec3 effect = PointLighting(position, uEyePos, uLight, normal) * fragColor;

	out_color = vec4(effect, 1.0);
}

