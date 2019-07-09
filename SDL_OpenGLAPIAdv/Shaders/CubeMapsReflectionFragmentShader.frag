#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 position;

out vec4 color;

uniform samplerCube sky;
uniform vec3 cameraPos;

void main()
{
	vec3 lookat = normalize(position - cameraPos);
	vec3 reflection = reflect(lookat, normalize(normal));

	vec4 texColor = texture(sky, reflection);
	
	color = vec4(texColor.rgb, 1.0);

}

