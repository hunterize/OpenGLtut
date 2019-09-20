#version 330

out vec4 color;

in vec3 fcolor;

void main()
{
	color = vec4(fcolor, 1.0);
}

