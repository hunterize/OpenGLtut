#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNor;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 vertexUV;

uniform mat4 model;
uniform	mat4 projection;
uniform	mat4 view;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

out mat3 TBN;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	texCoord = vertexUV;
	fragPos = vec3(model * vec4(vertexPos, 1.0));

	vec3 norm = vertexNor;

	normal = mat3(transpose(inverse(model))) * norm;

	vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
	vec3 N = normalize(cross(T, B));

	TBN = mat3(T, B, N);
}
