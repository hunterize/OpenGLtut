#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexColor;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

out mat4 matrix;

out VS_OUT
{
	vec3 vcolor;
} vs_out;

void main()
{
	gl_Position = model * vec4(vertexPos, 1.0);
	vs_out.vcolor = vertexColor;
	matrix = projection * view;
}

