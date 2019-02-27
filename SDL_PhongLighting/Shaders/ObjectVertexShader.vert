//
//calculate lighting in view space

#version 330

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;

out vec3 normal;
out vec3 fragPos;
out vec3 viewLightPos;

void main()
{
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
	fragPos = vec3(view * model * vec4(vertexPos, 1.0));
	viewLightPos = vec3(view * vec4(lightPos, 1.0));
	normal = mat3(transpose(inverse(view * model))) * vertexNor;
}