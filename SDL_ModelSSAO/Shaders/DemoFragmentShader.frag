#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D sample;

void main()
{
	color = vec4(0.0, 0.5, 0.5, 1.0);
}

