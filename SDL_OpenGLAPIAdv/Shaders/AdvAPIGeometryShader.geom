#version 330
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT
{
	vec3 vcolor;
} gs_in[];

in mat4 matrix[];

out vec3 fcolor;

//vertices order is the case
void Polygon()
{
	fcolor = gs_in[0].vcolor;
	
	//bottom left
	gl_Position = matrix[0] * (gl_in[0].gl_Position + vec4(-2.0, -2.0, 0.0, 0.0));
	EmitVertex();

	//bottom right
	gl_Position = matrix[0] * (gl_in[0].gl_Position + vec4(2.0, -2.0, 0.0, 0.0));
	EmitVertex();
	
	//top left
	gl_Position = matrix[0] * (gl_in[0].gl_Position + vec4(-2.0, 2.0, 0.0, 0.0));
	EmitVertex();

	//top right
	gl_Position = matrix[0] * (gl_in[0].gl_Position + vec4(2.0, 2.0, 0.0, 0.0));
	EmitVertex();

	//roof
	gl_Position = matrix[0] * (gl_in[0].gl_Position + vec4(0.0, 4.0, 0.0, 0.0));
	fcolor = vec3(1.0, 1.0, 1.0);
	EmitVertex();

	EndPrimitive();	
}

void main()
{
	Polygon();
}

