#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 position;

out vec4 color;

uniform samplerCube env;
uniform vec3 cameraPos;

void reflection()
{
	vec3 lookat = normalize(position - cameraPos);
	vec3 reflection = reflect(lookat, normalize(normal));

	vec4 texColor = texture(env, reflection);	
	color = vec4(texColor.rgb, 1.0);

}

void refraction()
{
	float ratio = 1.0/1.52;
	vec3 lookat = normalize(position - cameraPos);
	vec3 refract = refract(lookat, normalize(normal), ratio);

	vec4 texColor = texture(env, refract);
	
	color = vec4(texColor.rgb, 1.0);

}

void main()
{
	reflection();
	//refraction();
}

