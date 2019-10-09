#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNor;
layout (location = 2) in vec2 vertexUV;

layout (location = 12) in vec3 instancePos;

uniform mat4 model;

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

out vec2 texCoord;
flat out float iid;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos + instancePos, 1.0);
	texCoord = vertexUV;
	iid = gl_InstanceID / 1000.0f;
}
