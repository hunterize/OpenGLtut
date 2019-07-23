#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D outside;
uniform sampler2D inside;

void main()
{
	if(gl_FrontFacing)
		if(gl_FragCoord.x < 800)
			color = texture(outside, texCoord);
		else
			color = texture(outside, texCoord) * vec4(0.5, 0.5, 0.5, 1.0);
	else
		color = texture(inside, texCoord);

}

