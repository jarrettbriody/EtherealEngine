
#include "ParticleStructs.hlsli"

cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

StructuredBuffer<Particle>		ParticlePool			: register(t0);
StructuredBuffer<ParticleDrawInfo>	ParticleDrawList		: register(t1);

struct VStoPS
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
	int textureIndex : TEXINDEX;
};

VStoPS main(uint id : SV_VertexID)
{
	// Output struct
	VStoPS output;

	// Get id info
	uint particleID = id / 4;
	uint cornerID = id % 4;

	// Look up the draw info, then this particle
	ParticleDrawInfo drawInfo = ParticleDrawList.Load(particleID);
	Particle particle = ParticlePool.Load(drawInfo.index);

	// Offsets for triangles
	float angularOffsetX = cos(particle.rotationRadians);
	float angularOffsetY = sin(particle.rotationRadians);
	float2 offsets[4];
	offsets[0] = float2(-1.0f * angularOffsetX - 1.0f * angularOffsetY, -1.0f * angularOffsetY + 1.0f * angularOffsetX);  // TL
	offsets[1] = float2(+1.0f * angularOffsetX - 1.0f * angularOffsetY, 1.0f * angularOffsetY + 1.0f * angularOffsetX);  // TR
	offsets[2] = float2(+1.0f * angularOffsetX + 1.0f * angularOffsetY, 1.0f * angularOffsetY - 1.0f * angularOffsetX);  // BR
	offsets[3] = float2(-1.0f * angularOffsetX + 1.0f * angularOffsetY, -1.0f * angularOffsetY - 1.0f * angularOffsetX);  // BL

	// Calc position of this corner
	float3 pos = particle.position;
	pos = mul(mul(float4(pos, 1.0f), world), view);
	pos.xy += offsets[cornerID].xy * particle.scale;

	output.position = mul(float4(pos,1.0f), projection);

	float2 uvs[4];
	uvs[0] = float2(0, 0);  // TL
	uvs[1] = float2(1, 0);  // TR
	uvs[2] = float2(1, 1);  // BR
	uvs[3] = float2(0, 1);  // BL

	// Pass through
	output.color = particle.color;
	output.uv = saturate(uvs[cornerID]);
	output.textureIndex = particle.textureIndex;

	return output;
}