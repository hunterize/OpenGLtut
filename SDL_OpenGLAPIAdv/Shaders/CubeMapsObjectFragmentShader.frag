#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D sample;

void main()
{
	vec4 texColor = texture(sample, texCoord);
	
	if(texColor.a < 0.1)
	{
		discard;
	}

	color = texColor;

}

