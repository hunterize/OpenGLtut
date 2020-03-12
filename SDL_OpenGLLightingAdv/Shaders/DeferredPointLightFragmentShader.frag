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

void main()
{
	vec2 texCoord = GetTexCoord();

	vec3 position = texture(gPosition, texCoord).xyz;
	vec3 normal = normalize(texture(gNormal, texCoord).xyz);
	vec3 fragColor = texture(gAlbedoSpecular, texCoord).xyz;
	
	vec3 effect = BlinnPhongLighting(position, uLightPos, uEyePos, normal, fragColor);

	out_color = vec4(effect, 1.0);
}

