
#include "ParticleStructs.hlsli"

cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

// Describes individual vertex data
struct VertexShaderInput
{
	float3 position		: POSITION;
	float2 uv			: TEXCOORD;
	float4 color		: COLOR;
	float scale			: SCALE;
	int num				: ID;
	int textureIndex	: TEXINDEX;
};

struct VStoPS
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
	int textureIndex : TEXINDEX;
};

VStoPS main(VertexShaderInput input)
{
	// Output struct
	VStoPS output;

	// Offsets for triangles
	//float angularOffsetX = cos(particle.rotationRadians);
	//float angularOffsetY = sin(particle.rotationRadians);
	float2 offsets[4];
	offsets[0] = float2(-1.0f, +1.0f);  // TL
	offsets[1] = float2(+1.0f, +1.0f);  // TR
	offsets[2] = float2(+1.0f, -1.0f);  // BR
	offsets[3] = float2(-1.0f, -1.0f);  // BL

	// Calc position of this corner
	float3 pos = input.position;
	pos = mul(mul(float4(pos, 1.0f), world), view);
	pos.xy += offsets[input.num].xy * input.scale;

	output.position = mul(float4(pos, 1.0f), projection);

	// Pass through
	output.color = input.color;
	output.uv = input.uv;
	output.textureIndex = input.textureIndex;

	return output;
}