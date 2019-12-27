#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D hdrSample;
uniform float exposure;
uniform bool isHDR;

void HDR()
{
	vec4 texColor = texture(hdrSample, texCoord);
	float gamma = 2.2;
	vec3 effect = texColor.rgb;

	effect = vec3(1.0) - exp(vec3(-1.0) * effect * exposure);

	effect = pow(effect, vec3(1.0 / gamma));
	color = vec4(effect, 1.0);
}

void normal()
{
	vec4 texColor = texture(hdrSample, texCoord);
	float gamma = 2.2;
	vec3 effect = texColor.rgb;
	effect = pow(effect, vec3(1.0 / gamma));
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



