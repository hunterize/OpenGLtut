#version 330

flat in float iid;
in vec2 texCoord;
out vec4 color;

uniform sampler2D outside;

void main()
{
	color = texture(outside, texCoord);
	//color = vec4(0.0, 1.0, 0.0, 1.0);
}

