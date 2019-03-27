#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D texture_diffuse1;

void main()
{
	color = texture(texture_diffuse1, texCoord);
}