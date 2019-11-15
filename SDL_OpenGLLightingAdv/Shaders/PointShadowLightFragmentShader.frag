#version 330

in vec2 texCoord;
out vec4 finalColor;

uniform sampler2D sample;

void main()
{
	finalColor = vec4(1.0f);
}

