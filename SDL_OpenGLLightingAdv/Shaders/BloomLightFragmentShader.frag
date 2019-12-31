#version 330

uniform vec3 lightColor;
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 brightColor;

void main()
{
	color = vec4(lightColor, 1.0f);

	float brightness = dot(color.rgb, vec3(02126, 0.7152, 0.0722));
	if(brightness > 1.0)
	{
		brightColor = vec4(color.rgb, 1.0);
	}
	else
	{
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}