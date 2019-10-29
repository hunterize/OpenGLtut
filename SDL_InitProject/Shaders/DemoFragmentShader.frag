#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D sample;

void main()
{
	color = texture(sample, texCoord);
}

