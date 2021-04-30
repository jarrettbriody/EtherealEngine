
cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float totalTime;
	float deltaTime;
	float waveHeightX;
	float waveHeightY;
	float waveHeightZ;
	float waveRateX;
	float waveRateY;
	float waveRateZ;
};

struct VertexShaderInput
{
	float3 position		: POSITION;     // XYZ position
	float3 normal       : NORMAL;
	float2 uv           : TEXCOORD;
	float3 tangent		: TANGENT;
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal       : NORMAL;
	float2 uv           : TEXCOORD;
	float3 worldPos		: POSITION;
	float3 tangent		: TANGENT;
};

VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	float3x3 worldNoTranslation = (float3x3)world;

	matrix worldViewProj = mul(mul(world, view), projection);

	float3 newPos = input.position;

	if (newPos.y > 0.0f) {
		float fluxX = (sin(newPos.x + totalTime * waveRateX)) * waveHeightX;
		float fluxY = ((cos(newPos.y + totalTime * waveRateY) - 1.0f)) * waveHeightY;
		float fluxZ = (sin(newPos.y + totalTime * waveRateZ)) * waveHeightZ;
		newPos += float3(fluxX, fluxY, fluxZ);
	}

	output.worldPos = mul(float4(newPos, 1.0f), world);

	output.position = mul(float4(newPos, 1.0f), worldViewProj);

	output.normal = normalize(mul(input.normal, worldNoTranslation));

	output.uv = input.uv;

	return output;
}