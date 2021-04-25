
cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	//matrix shadowView;
	//matrix shadowProj;
	float fillLineY;
	float totalTime;
	float deltaTime;
	float waveHeight;
	int counter;
	//float maxY;
	//float minY;
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
	//float4 posForShadow : SHADOW;
};

VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	//matrix worldViewProj = mul(mul(world, view), projection);

	float3x3 worldNoTranslation = (float3x3)world;

	float3 newPos = mul(input.position, worldNoTranslation);

	float flux = (sin(newPos.x * 5.0f + totalTime)) * waveHeight;//((sin(totalTime) - 0.5f) / 2.0f + (sin(newPos.x + counter / 50.0f) - 0.5f) / 2.0f) * waveHeight;

	if (newPos.y > fillLineY) newPos.y = fillLineY + flux;

	output.worldPos = float3(newPos.x + world._41, newPos.y + world._42, newPos.z + world._43);

	output.position = mul(float4(mul(float4(output.worldPos, 1.0f), view).xyz,1.0f), projection);

	output.normal = normalize(mul(input.normal, worldNoTranslation));

	output.uv = input.uv;


	// Calculate shadow map position
	//matrix shadowWVP = mul(mul(world, shadowView), shadowProj);

	//output.posForShadow = mul(float4(mul(float4(output.worldPos, 1.0f), shadowView).xyz, 1.0f), shadowProj);

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}