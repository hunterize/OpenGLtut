#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNor;
layout (location = 2) in vec2 vertexUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT
{
	vec2 texCoord;
	vec3 normal;
	vec3 fragPos;
	mat4 matrix;
} vs_out;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	vs_out.texCoord = vertexUV;
	//vertex position in world space
	vs_out.fragPos = vec3(model * vec4(vertexPos, 1.0));
	vs_out.normal = mat3(transpose(inverse(model))) * vertexNor;
	vs_out.matrix = projection * view;
}