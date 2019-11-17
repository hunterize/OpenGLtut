#version 330

//fragment shader for depth cube map,
//calculate linear depth values

in vec4 fragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
	float lightDis = length(fragPos.xyz - lightPos);
	lightDis = lightDis / far_plane;
	gl_FragDepth = lightDis;
}

