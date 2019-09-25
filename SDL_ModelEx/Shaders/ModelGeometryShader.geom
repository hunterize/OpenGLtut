#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

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

vec4 Inflate(int index)
{
	texCoord = gs_in[index].texCoord;
	normal = gs_in[index].normal;
	fragPos = gs_in[index].fragPos;

	return gl_in[index].gl_Position + gs_in[index].matrix * vec4(gs_in[index].normal * 0.6, 0.0);
}

vec4 Explode(int index)
{
	texCoord = gs_in[index].texCoord;
	normal = gs_in[index].normal;
	fragPos = gs_in[index].fragPos;
	
	//get exploding normal, assimp vertices are clockwise wilding?
	vec3 line1 = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 line2 = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 normal = normalize(cross(line1, line2));
	return gl_in[index].gl_Position + gs_in[index].matrix * vec4(normal * 0.3, 0.0);
}


vec4 Bypass(int index)
{
	texCoord = gs_in[index].texCoord;
	normal = gs_in[index].normal;
	fragPos = gs_in[index].fragPos;
	return gl_in[index].gl_Position;
}

void NonEffect()
{
	gl_Position = Bypass(0);
	EmitVertex();

	gl_Position = Bypass(1);
	EmitVertex();

	gl_Position = Bypass(2);
	EmitVertex();

	EndPrimitive();
}

void ExplodeEffect()
{
	gl_Position = Explode(0);
	EmitVertex();

	gl_Position = Explode(1);
	EmitVertex();

	gl_Position = Explode(2);
	EmitVertex();

	EndPrimitive();	
}

void InflateEffect()
{
	gl_Position = Inflate(0);
	EmitVertex();

	gl_Position = Inflate(1);
	EmitVertex();

	gl_Position = Inflate(2);
	EmitVertex();

	EndPrimitive();
}

void main()
{
	//InflateEffect();
	//ExplodeEffect();
	NonEffect();
}