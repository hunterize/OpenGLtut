#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D texture1;

void main()
{
	vec4 texColor = texture(texture1, texCoord);
	
	if(texColor.a < 0.1)
	{
		discard;
	}

	color = texColor;

}

