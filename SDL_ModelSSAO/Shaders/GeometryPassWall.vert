#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNor;
layout (location = 2) in vec2 vertexUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool isNormalInverted;

out	vec2 texCoord;
out	vec3 normal;
out	vec3 fragPos;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	texCoord = vertexUV;

	//vertex position in view space
	fragPos = vec3(view * model * vec4(vertexPos, 1.0));

	//normal vector in view space
	normal = mat3(transpose(inverse(view * model))) * (isNormalInverted? -vertexNor : vertexNor);

}