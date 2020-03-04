#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNor;
layout (location = 2) in vec3 vertexTan;
layout (location = 3) in vec2 vertexUV;

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

	vec3 norm = mat3(transpose(inverse(model))) * vertexNor;
	normal = normalize(norm);

	vec3 T = normalize(vec3(model * vec4(vertexTan, 0.0)));
	vec3 N = normalize(norm);
	vec3 B = normalize(cross(N, T));
	TBN = mat3(T, B, N);
}
