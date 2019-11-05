#version 330

out vec4 color;
in vec2 texCoord;
uniform sampler2D depthMap;

void main()
{
	float depthValue = texture(depthMap, texCoord).r;
	color = vec4(vec3(depthValue), 1.0);
}

