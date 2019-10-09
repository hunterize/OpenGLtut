#version 330

flat in float iid;
in vec2 texCoord;
out vec4 color;

uniform sampler2D outside;

void main()
{
	//color = vec4(abs(sin(iid)), 1- abs(sin(iid)), abs(cos(iid)), 1.0) *  texture(outside, texCoord);
	color = texture(outside, texCoord);
}

