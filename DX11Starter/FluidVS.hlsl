
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
	float radius;
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
	flux += ((cos(newPos.z * 10.0f + totalTime)) * (waveHeight / 2.0f));
	//flux += ((cos(newPos.y + totalTime)) * (waveHeight / 5.0f)) + waveHeight / 5.0f;
	if (newPos.y >= (fillLineY + flux)) newPos.y = fillLineY + flux;

	/*
	float hypotenuse = length(newPos);
	if (hypotenuse > radius) {
		float deltaHypot = (hypotenuse - radius) * 1.003f;
		float angle = asin(newPos.y / hypotenuse);
		float deltaDist = sin(angle) * deltaHypot;
		float xMultiplier = (newPos.x < 0) ? -1.0f : 1.0f;
		float zMultiplier = (newPos.z < 0) ? -1.0f : 1.0f;
		newPos = float3(newPos.x + deltaDist * xMultiplier * cos(3.14159265f / 4.0f), newPos.y, newPos.z + deltaDist * zMultiplier * sin(3.14159265f / 4.0f));
	}
	*/

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