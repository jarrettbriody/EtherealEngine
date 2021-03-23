cbuffer externalData : register(b0) {
	float3 cameraPosition;
}

// Out of the vertex shader (and eventually input to the PS)
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 worldPosition : POSITION;
};

float main(VertexToPixel input) : SV_TARGET
{
	return length(input.worldPosition - cameraPosition);
}