
#include "ParticleStructs.hlsli"

struct VStoPS
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD0;
	int textureIndex : TEXINDEX;
};

Texture2DArray particleTextures : register(t0);
SamplerState Sampler			: register(s0);

float4 main(VStoPS input) : SV_TARGET
{
	float3 color;
	if (input.textureIndex != -1) {
		color = particleTextures.Sample(Sampler, float3(input.uv, input.textureIndex));
	}
	else {
		// Distance from center
		// Convert uv to -1 to 1
		input.uv = input.uv * 2 - 1;
		float fade = saturate(distance(float2(0, 0), input.uv));
		color = lerp(input.color.rgb, float3(0, 0, 0), fade * fade);
	}

	return float4(color, 1);
}