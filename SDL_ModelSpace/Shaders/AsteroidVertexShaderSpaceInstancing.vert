#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNor;
layout (location = 2) in vec2 vertexUV;
layout (location = 6) in mat4 instanceModel;

layout (std140) uniform astMatrices
{
	mat4 projection;
	mat4 view;
};

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

void main()
{
	gl_Position = projection * view * instanceModel * vec4(vertexPos, 1.0);
	texCoord = vertexUV;
	//vertex position in world space
	fragPos = vec3(instanceModel * vec4(vertexPos, 1.0));
	normal = mat3(transpose(inverse(instanceModel))) * vertexNor;
}