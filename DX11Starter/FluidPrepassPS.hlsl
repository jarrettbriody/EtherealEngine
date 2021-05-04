cbuffer externalData : register(b0) {
	float3 cameraPosition;
	unsigned int entityLayerMask;
	float3 orbCenter;
}

// Out of the vertex shader (and eventually input to the PS)
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 worldPosition : POSITION;
};

struct GBuffer
{
	float  depth				 : SV_TARGET0;
	unsigned int entityLayerMask : SV_TARGET1;
};

GBuffer main(VertexToPixel input)
{
	float3 pixDist = input.worldPosition - orbCenter;
	if (length(pixDist) > 0.505f) discard;
	GBuffer output;
	output.depth = length(input.worldPosition - cameraPosition);
	output.entityLayerMask = entityLayerMask;
	return output;
}