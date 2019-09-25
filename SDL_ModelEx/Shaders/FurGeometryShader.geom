#version 330

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT
{
	vec2 texCoord;
	vec3 normal;
    vec3 fragPos;
	mat4 matrix;
} gs_in[];

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

vec4 Bypass(int index)
{
	texCoord = gs_in[index].texCoord;
	normal = gs_in[index].normal;
	fragPos = gs_in[index].fragPos;
	return gl_in[index].gl_Position;
}

void GetNormalLine(int index)
{
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();

	gl_Position = gl_in[index].gl_Position + gs_in[index].matrix * vec4(gs_in[index].normal * 0.12, 0.0);
	EmitVertex();
	EndPrimitive();
}

void main()
{
	GetNormalLine(0);
	GetNormalLine(1);
	GetNormalLine(2);
}