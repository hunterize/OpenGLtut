#version 330

//geometry shader for depth cube map

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 cubeMatrices[6];

out vec4 fragPos;

void main()
{
	for(int face = 0; face < 6; face++)
	{
		gl_Layer = face;

		for(int i = 0; i < 3; i++)
		{
			fragPos = gl_in[i].gl_Position;
			gl_Position = cubeMatrices[face] * fragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}

