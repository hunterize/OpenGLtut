#version 330

out float ao;

in vec2 texCoord;
uniform vec3 samples[64];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;
uniform mat4 projection;
 
 int kernelSize = 64;
 float radius = 0.5;
 float bias = 0.025;

 const vec2 noiseScale = vec2(1600.0 / 4.0, 900.0 / 4.0);

 void SSAO()
 {
	vec3 fragPos = texture(gPosition, texCoord).xyz;
	vec3 normal = normalize(texture(gNormal, texCoord).xyz);
	vec3 rotationVec = normalize(texture(noiseTexture, texCoord * noiseScale).xyz);

	vec3 t = normalize(rotationVec - normal * dot(rotationVec, normal));
	vec3 b = cross(normal, t);
	mat3 TBN = mat3(t, b, normal);

	float occlusion = 0.0;
	for(int i = 0;  i< kernelSize; i++)
	{
		vec3 samplePos = TBN * samples[i];
		samplePos = fragPos + samplePos * radius;

		vec4 offset = vec4(samplePos, 1.0);

		//transform to clip space
		offset = projection * offset;
		
		offset.xyz /= offset.w;
		
		//transfrom to range {0.0, 1.0}
		offset.xyz = offset.xyz * 0.5 + 0.5;
		
		//get depth value of kernel sample
		float sampleDepth = texture(gPosition, offset.xy).z;
		
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));

		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / kernelSize);

	ao = occlusion;
 }

void main()
{
	SSAO();
}

