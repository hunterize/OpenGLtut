#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D hdrSample;
uniform float exposure;
uniform bool isHDR;

void HDR()
{
	vec4 texColor = texture(hdrSample, texCoord);
	vec3 effect = texColor.rgb;

	effect = vec3(1.0) - exp(vec3(-1.0) * effect * exposure);
	color = vec4(effect, 1.0);
}

void normal()
{
	vec4 texColor = texture(hdrSample, texCoord);
	vec3 effect = texColor.rgb;
	color = vec4(effect, 1.0);
}

void inverse()
{
	vec4 texColor = vec4(vec3(1.0 - texture(hdrSample, texCoord)), 1.0);
	color = texColor;
}

void main()
{
	if(isHDR)
	{
		HDR();
	}
	else
	{
		normal();
	}

}



