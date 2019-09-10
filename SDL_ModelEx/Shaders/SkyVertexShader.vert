#version 330

layout (location = 0) in vec3 vertexPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 texCoord;

void main()
{
	vec4 pos = projection * view * vec4(vertexPos, 1.0);
	gl_Position = pos;
	texCoord = vertexPos;
}

