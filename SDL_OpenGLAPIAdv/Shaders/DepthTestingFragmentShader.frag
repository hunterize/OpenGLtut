#version 330

in vec2 texCoord;
out vec4 color;

uniform sampler2D texture1;

float near = 1.0f;
float far = 100.0f;

float GetLinearDepthValue(float depth)
{
	//transfor to linear value
	float z = (far * near) / ((near - far) * depth + far);
	//clamp to (0,1)
	return (z - near)/(far-near);
}

float GetDepthValue(float depth)
{
	//clamp to NDC
	float z = 2.0 * depth - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	//color = texture(texture1, texCoord);	
	//color = vec4(vec3(gl_FragCoord.z), 1.0);
	//color = vec4(vec3(GetDepthValue(gl_FragCoord.z) / far), 1.0);	
	color = vec4(vec3(GetLinearDepthValue(gl_FragCoord.z)), 1.0);
}