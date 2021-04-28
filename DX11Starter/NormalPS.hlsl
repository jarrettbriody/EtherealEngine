#include "Lighting.hlsli"

#define MAX_LIGHTS 32


// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal       : NORMAL;
	float2 uv           : TEXCOORD;
	float3 worldPos		: POSITION;
	float3 tangent		: TANGENT;
	//float4 posForShadow : SHADOW;
};


cbuffer lightCBuffer : register(b0)
{
	Light lights[MAX_LIGHTS];
	int lightCount;
};

cbuffer uvRepeatCBuffer : register(b1) {
	float2 uvMult;
	float2 uvOffset;
};

cbuffer externalData : register(b2) {

	int specularValue;
	float3 cameraPosition;
	int illumination;
	float3 manualColor;
	float transparency;
}

cbuffer shadowStuff : register(b3) {
	float3 sunPos;
	float2 cascadeRange0;
	float2 cascadeRange1;
	float2 cascadeRange2;
	float2 cascadeRange3;
	matrix shadowView;
	matrix shadowProj0;
	matrix shadowProj1;
	matrix shadowProj2;
	matrix shadowProj3;
};

Texture2D DiffuseTexture  : register(t0);

Texture2D NormalTexture   : register(t1);

Texture2D ShadowMap0		  : register(t2);
Texture2D ShadowMap1		  : register(t3);
Texture2D ShadowMap2		  : register(t4);
Texture2D ShadowMap3		  : register(t5);

SamplerState BasicSampler               : register(s0);
SamplerComparisonState ShadowSampler	: register(s1);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.uv = float2(input.uv.x * uvMult.x + uvOffset.x, input.uv.y * uvMult.y + uvOffset.y);

	// Sample the normal from the normal map (remember to unpack it)
	float3 normalFromMap = NormalTexture.Sample(BasicSampler, input.uv).rgb * 2 - 1;

	// Calculate the matrix we'll use to convert from tangent to world space
	float3 N = normalize(input.normal);
	float3 T = normalize(input.tangent - N * dot(normalize(input.tangent), N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);

	// Use the normal from the map, after we've converted it to world space
	input.normal = normalize(mul(normalFromMap, TBN));

	float4 surfaceColor = DiffuseTexture.Sample(BasicSampler, input.uv);
	if (surfaceColor.a <= 0.001f) discard;

	if (illumination == 11) {
		surfaceColor = surfaceColor.xyzw - (1 - float4(manualColor.xyz, 0));
	}
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering

	// Shadow calculations
	float3 vec = mul(float4(input.worldPos - sunPos, 0.0f), shadowView);
	//return float4(distToSun / 1000, 0.0f, 0.0f, 1.0f);
	float shadowAmount = 1.0f;
	if (vec.x >= -cascadeRange0.x / 2.001f && vec.x <= cascadeRange0.x / 2.001f && vec.y >= -cascadeRange0.y / 2.001f && vec.y <= cascadeRange0.y / 2.001f) {
		//return float4(1.0f, 0.0f, 0.0f, 1.0f);
		float4 posForShadow = mul(mul(float4(input.worldPos, 1.0f), shadowView), shadowProj0);
		float2 shadowUV = ((posForShadow.xy / posForShadow.w) * 0.5f) + 0.5f;
		shadowUV.y = 1.0f - shadowUV.y;

		float depthFromLight = posForShadow.z / posForShadow.w;

		shadowAmount = ShadowMap0.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
	}
	else if (vec.x >= -cascadeRange1.x / 2.001f && vec.x <= cascadeRange1.x / 2.001f && vec.y >= -cascadeRange1.y / 2.001f && vec.y <= cascadeRange1.y / 2.001f) {
		//return float4(0.0f, 1.0f, 0.0f, 1.0f);
		float4 posForShadow = mul(mul(float4(input.worldPos, 1.0f), shadowView), shadowProj1);
		float2 shadowUV = ((posForShadow.xy / posForShadow.w) * 0.5f) + 0.5f;
		shadowUV.y = 1.0f - shadowUV.y;

		float depthFromLight = posForShadow.z / posForShadow.w;

		shadowAmount = ShadowMap1.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
	}
	else if (vec.x >= -cascadeRange2.x / 2.001f && vec.x <= cascadeRange2.x / 2.001f && vec.y >= -cascadeRange2.y / 2.001f && vec.y <= cascadeRange2.y / 2.001f) {
		//return float4(0.0f, 0.0f, 1.0f, 1.0f);
		float4 posForShadow = mul(mul(float4(input.worldPos, 1.0f), shadowView), shadowProj2);
		float2 shadowUV = ((posForShadow.xy / posForShadow.w) * 0.5f) + 0.5f;
		shadowUV.y = 1.0f - shadowUV.y;

		float depthFromLight = posForShadow.z / posForShadow.w;

		shadowAmount = ShadowMap2.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
	}
	else if (vec.x >= -cascadeRange3.x / 2.001f && vec.x <= cascadeRange3.x / 2.001f && vec.y >= -cascadeRange3.y / 2.001f && vec.y <= cascadeRange3.y / 2.001f) {
		//return float4(1.0f, 0.0f, 1.0f, 1.0f);
		float4 posForShadow = mul(mul(float4(input.worldPos, 1.0f), shadowView), shadowProj3);
		float2 shadowUV = ((posForShadow.xy / posForShadow.w) * 0.5f) + 0.5f;
		shadowUV.y = 1.0f - shadowUV.y;

		float depthFromLight = posForShadow.z / posForShadow.w;

		shadowAmount = ShadowMap3.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
	}

	float3 toCameraVector = normalize(cameraPosition - input.worldPos);


	float3 finalColor = float3(0.f, 0.f, 0.f);
	for (int i = 0; i < lightCount; i++)
	{
		switch (lights[i].Type) {
		case LIGHT_TYPE_DIR:
			finalColor += (CalcDirectionalLight(surfaceColor, input.normal, lights[i], toCameraVector, specularValue, shadowAmount));
			break;
		case LIGHT_TYPE_POINT:
			finalColor += (CalcPointLight(surfaceColor, input.normal, lights[i], toCameraVector, specularValue, input.worldPos));
			break;
		case LIGHT_TYPE_SPOT:
			finalColor += (CalcSpotLight(surfaceColor, input.normal, lights[i], toCameraVector, specularValue, input.worldPos));
			break;
		}
	}

	float3 gammaCorrect = pow(abs(finalColor), 1.0f / 2.2f);

	return float4(gammaCorrect, surfaceColor.a - (1.0f - transparency));
}