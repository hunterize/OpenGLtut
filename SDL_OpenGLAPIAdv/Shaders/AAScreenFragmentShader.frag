#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D screenSample;
uniform int eff;

void normal()
{
	vec4 texColor = texture(screenSample, texCoord);
	color = texColor;
}

void inverse()
{
	vec4 texColor = vec4(vec3(1.0 - texture(screenSample, texCoord)), 1.0);
	color = texColor;
}

void grayscale()
{
	vec4 texColor = texture(screenSample, texCoord);
	//float av = (texColor.r + texColor.g + texColor.b) / 3.0;
	float av = texColor.r * 0.2126 + texColor.g * 0.7152 + texColor.b * 0.0722;
	color = vec4(av, av, av, 1.0);
}

//kernel effect

const float offset = 1.0 / 600.0;

vec2 offsets[9] = vec2[](
	vec2(-offset, offset),
	vec2(0.0,     offset),
	vec2(offset,  offset),
	vec2(-offset, 0.0),
	vec2(0.0,     0.0),
	vec2(offset,  0.0),
	vec2(-offset, -offset),
	vec2(0.0,     -offset),
	vec2(offset,  -offset)
);


void sharpen()
{
	float kernel[9] = float[](
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	);

	vec3 sampleTex[9];

	for(int i=0; i<9; i++)
	{
		sampleTex[i] = vec3(texture(screenSample, texCoord.st + offsets[i]));
	}

	vec3 col = vec3(0.0);

	for(int i=0; i<9; i++)
	{
		col += sampleTex[i] * kernel[i];
	}

	color = vec4(col, 1.0);

}

void blur()
{
	float kernel[9] = float[](
	1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
	2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
	1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
	);

	vec3 sampleTex[9];

	for(int i=0; i<9; i++)
	{
		sampleTex[i] = vec3(texture(screenSample, texCoord.st + offsets[i]));
	}

	vec3 col = vec3(0.0);

	for(int i=0; i<9; i++)
	{
		col += sampleTex[i] * kernel[i];
	}

	color = vec4(col, 1.0);
}

void edge()
{
	float kernel[9] = float[](
	1.0, 1.0, 1.0,
	1.0, -8.0, 1.0,
	1.0, 1.0, 1.0
	);

	vec3 sampleTex[9];

	for(int i=0; i<9; i++)
	{
		sampleTex[i] = vec3(texture(screenSample, texCoord.st + offsets[i]));
	}

	vec3 col = vec3(0.0);

	for(int i=0; i<9; i++)
	{
		col += sampleTex[i] * kernel[i];
	}

	color = vec4(col, 1.0);
}

void main()
{
	switch(eff)
	{
		case 1:
			normal();
			break;
		case 2:
			inverse();
			break;
		case 3:
			grayscale();
			break;
		case 4:
			sharpen();
			break;
		case 5:
			blur();
			break;
		case 6:
			edge();
			break;
		default:
			normal();
			break;
	}
}



