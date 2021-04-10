cbuffer externalData : register(b0) {
	float3 cameraPosition;
	uint entityInfo;
}

// Out of the vertex shader (and eventually input to the PS)
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 worldPosition : POSITION;
};

struct GBuffer
{
	float  depth				: SV_TARGET0;
	uint entityInfo				: SV_TARGET1;
};

GBuffer main(VertexToPixel input)
{
	GBuffer output;
	output.depth = length(input.worldPosition - cameraPosition);
	output.entityInfo = entityInfo;
	return output;
}