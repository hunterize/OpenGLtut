#version 330

uniform vec3 fragmentColor;
uniform vec3 lightColor;
out vec4 color;		
void main()
{
	color = vec4(fragmentColor*lightColor, 1.0f);
}