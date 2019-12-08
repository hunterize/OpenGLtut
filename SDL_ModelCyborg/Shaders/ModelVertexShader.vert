#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNor;
layout (location = 2) in vec2 vertexUV;
layout (location = 3) in vec3 vertexTangent;
layout (location = 4) in vec3 vertexBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out	vec2 texCoord;
out	vec3 normal;
out	vec3 fragPos;

out mat3 TBN;


void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	texCoord = vertexUV;
	//vertex position in world space
	fragPos = vec3(model * vec4(vertexPos, 1.0));
	normal = mat3(transpose(inverse(model))) * vertexNor;

	vec3 T = normalize(vec3(model * vec4(vertexTangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(vertexBitangent, 0.0)));

	vec3 nor = normalize(cross(T, B));

	if(dot(nor, normalize(vec3(model * vec4(vertexNor, 0.0)))) < 0.0)
	{
		T = (-1.0) * T;
	}

	vec3 N = normalize(cross(T, B));

	TBN = mat3(T, B, N);
}