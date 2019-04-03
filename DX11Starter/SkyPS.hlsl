struct VertexToPixel {
	float4 position			: SV_POSITION;
	float3 cubeDirection	: DIRECTION;
};

TextureCube Sky				: register(t0);
SamplerState BasicSampler	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	return Sky.Sample(BasicSampler, input.cubeDirection);
}