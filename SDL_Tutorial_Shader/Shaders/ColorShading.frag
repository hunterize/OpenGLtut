#version 130

in vec2 fragmentPosition;
in vec4 fragmentColor;
in vec2 fragmentUV;

out vec4 color;

uniform float time;
uniform sampler2D mySampler;

void main()
{
	//color = vec3(1.0, 0.0, 0.0);
	//color = fragmentColor + vec4(1.0 * (cos(time) + 1.0) * 0.5,
	//							1.0 * (cos(time) + 2.0) * 0.5,
	//							1.0 * (cos(time) + 1.0) * 0.5,
	//							0.0);
	
	
	//make amazing coloers using time and postition
	color = vec4(fragmentColor.r * (cos(fragmentPosition.x * 4 + time) + 1.0) * 0.5,
				 fragmentColor.g * (cos(fragmentPosition.y * 8 + time) + 1.0) * 0.5,
				 fragmentColor.b * (cos(fragmentPosition.x * 2 + time) + 1.0) * 0.5,
				 fragmentColor.a
				 );
				 
	vec4 textureColor = texture(mySampler, fragmentUV);
	color = textureColor * color;
}