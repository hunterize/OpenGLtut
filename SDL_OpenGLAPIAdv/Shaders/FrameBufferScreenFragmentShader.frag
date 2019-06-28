#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D screenSample;

void main()
{
	vec4 texColor = texture(screenSample, texCoord);
	
	color = texColor;

}

