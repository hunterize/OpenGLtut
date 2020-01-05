#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D image;
uniform bool isHo;

uniform float weight[5] = float[] (0.2270270270, 0.19455945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
	vec2 offset = 1.0 / textureSize(image, 0);
	vec3 result = texture(image, texCoord).rgb * weight[0];
	
	if(isHo)
	{
		for(int i = 1; i < 5; i++)
		{
			result += texture(image, texCoord + vec2(offset.x * i, 0.0)).rgb * weight[i];
			result += texture(image, texCoord - vec2(offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 5; i++)
		{
			result += texture(image, texCoord + vec2(0.0, offset.y * i)).rgb * weight[i];
			result += texture(image, texCoord - vec2(0.0, offset.y * i)).rgb * weight[i];
		}
	}

	color = vec4(result, 1.0);
}



