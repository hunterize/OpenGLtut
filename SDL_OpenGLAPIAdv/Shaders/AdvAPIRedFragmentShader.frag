#version 330

out vec4 color;

void main()
{
	if(gl_FragCoord.x < 800)
		color = vec4(1.0, 0.0, 0.0, 1.0);
	else
		color = vec4(0.5, 0.0, 0.0, 1.0);
}

