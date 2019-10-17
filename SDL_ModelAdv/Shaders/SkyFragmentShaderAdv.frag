#version 330

in vec3 texCoord;
out vec4 color;

uniform samplerCube skybox;

void main()
{
	vec4 texColor = texture(skybox, texCoord);
	
	color = texColor;

}

