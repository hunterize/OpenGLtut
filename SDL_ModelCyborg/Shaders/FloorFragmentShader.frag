#version 330

in vec2 texCoord;
in	vec3 normal;
in	vec3 fragPos;

in mat3 TBN;
out vec4 color;

uniform sampler2D floorTex;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 eyePos;
uniform float shininess;

void BlinnPhongLighting()
{
	vec3 norm = texture(normalMap, texCoord).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	norm = normalize(TBN * norm);

	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(floorTex, texCoord));

	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 halfVector = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
	vec3 specular = spec * vec3(0.6);

	vec3 ambient = 0.2f * vec3(texture(floorTex, texCoord));

	vec3 effect = lightColor * (ambient + diffuse + specular);

	color = vec4(effect, 1.0);

}

void main()
{
	BlinnPhongLighting();
}

