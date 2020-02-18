#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

in mat3 TBN;

out vec4 finalColor;


uniform sampler2D sample;
uniform sampler2D nmap;

uniform vec3 eyePos;
uniform vec3 lightPos;
uniform float shininess;

struct Light
{
	vec3 position;
	vec3 color;
};

uniform Light lights[4];

//Blinn-Phong lighting
void BlinnPhongLighting()
{	

	vec3 lighting = vec3(0.0);

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);
	vec3 ambient = vec3(0.0);


	float distance = length(lightPos - fragPos);
	float attenuation = 1.0 / (1.0 + 0.02 * distance + 0.001 * distance * distance);

	//vec3 norm = normalize(normal);
	vec3 norm = texture(nmap, texCoord).rgb;
	norm = normalize(norm * 2.0 -1.0);
	norm = normalize(TBN * norm);

	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	diffuse +=  diff * vec3(texture(sample, texCoord));
	
	if(dot(normalize(normal), lightDir) > 0.0)
	{
		vec3 viewDir = normalize(eyePos - fragPos);
		vec3 halfVector = normalize(lightDir + viewDir);
		float spec = pow(max(dot(norm, halfVector), 0.0), shininess);
		specular +=  spec * vec3(0.6);
	}

	float ambi = 0.2f;
	ambient +=  ambi * vec3(texture(sample, texCoord));

	vec3 effect = ambient + diffuse + specular;

	finalColor = vec4(effect, 1.0);
}

void BasicColor()
{
	finalColor = texture(sample, texCoord);
}

void main()
{
	//BasicColor();
	BlinnPhongLighting();
}

