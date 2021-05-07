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
	//float2 uv           : TEXCOORD;
	float3 worldPos		: POSITION;
	float3 tangent		: TANGENT;
	float3 viewRay      : TEXCOORD0;
};

cbuffer lightCBuffer : register(b0)
{
	Light lights[MAX_LIGHTS];
	int lightCount;
};

cbuffer externalData : register(b1) {
	int illumination;
	//float brightness;
	int specularValue;
	float3 manualColor;
	matrix worldMatrix;
	matrix inverseWorldMatrix;
};

cbuffer constantPerFrame : register(b2) {
	float3 cameraPos;
	unsigned int decalLayerMask;
};

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

Texture2D DepthBuffer	  : register(t0);
Texture2D Decal			  : register(t1);
Texture2D<uint> EntityInfoBuffer	  : register(t2);
Texture2D ShadowMap0		  : register(t3);
Texture2D ShadowMap1		  : register(t4);
Texture2D ShadowMap2		  : register(t5);
Texture2D ShadowMap3		  : register(t6);

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
	float3 pixelIndex = float3(input.position.xy, 0);

	unsigned int layer = EntityInfoBuffer.Load(pixelIndex).r;

	if ((decalLayerMask | layer) != layer) discard;

	float  depth = DepthBuffer.Load(pixelIndex).r;

	input.viewRay = normalize(input.viewRay);

	float3 worldPos = cameraPos + input.viewRay * depth;

	float3 objPos = mul(float4(worldPos, 1.0f), inverseWorldMatrix);

	clip(0.5f - abs(objPos.xyz));

	//return float4(1.0f, 0.0f, 0.0f, 1.0f);

	float2 decalUV = objPos.xy + 0.5f;

	float4 surfaceColor = Decal.Sample(BasicSampler, decalUV);

	//float3 decalUV = float3((objPos.xy + 0.5f) * 512, 0);

	//float4 surfaceColor = Decal.Load(decalUV);

	if (surfaceColor.a < 0.3f) discard;

	if (illumination == 11) {
		surfaceColor = surfaceColor.xyzw - (1 - float4(manualColor.xyz, 0));
	}

	// Shadow calculations
	float3 vec = mul(float4(worldPos - sunPos, 0.0f), shadowView);
	//return float4(distToSun / 1000, 0.0f, 0.0f, 1.0f);
	float shadowAmount = 1.0f;
	if (vec.x >= -cascadeRange0.x / 2.001f && vec.x <= cascadeRange0.x / 2.001f && vec.y >= -cascadeRange0.y / 2.001f && vec.y <= cascadeRange0.y / 2.001f) {
		//return float4(1.0f, 0.0f, 0.0f, 1.0f);
		float4 posForShadow = mul(mul(float4(worldPos, 1.0f), shadowView), shadowProj0);
		float2 shadowUV = ((posForShadow.xy / posForShadow.w) * 0.5f) + 0.5f;
		shadowUV.y = 1.0f - shadowUV.y;

		float depthFromLight = posForShadow.z / posForShadow.w;

		shadowAmount = ShadowMap0.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
	}
	else if (vec.x >= -cascadeRange1.x / 2.001f && vec.x <= cascadeRange1.x / 2.001f && vec.y >= -cascadeRange1.y / 2.001f && vec.y <= cascadeRange1.y / 2.001f) {
		//return float4(0.0f, 1.0f, 0.0f, 1.0f);
		float4 posForShadow = mul(mul(float4(worldPos, 1.0f), shadowView), shadowProj1);
		float2 shadowUV = ((posForShadow.xy / posForShadow.w) * 0.5f) + 0.5f;
		shadowUV.y = 1.0f - shadowUV.y;

		float depthFromLight = posForShadow.z / posForShadow.w;

		shadowAmount = ShadowMap1.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
	}
	else if (vec.x >= -cascadeRange2.x / 2.001f && vec.x <= cascadeRange2.x / 2.001f && vec.y >= -cascadeRange2.y / 2.001f && vec.y <= cascadeRange2.y / 2.001f) {
		//return float4(0.0f, 0.0f, 1.0f, 1.0f);
		float4 posForShadow = mul(mul(float4(worldPos, 1.0f), shadowView), shadowProj2);
		float2 shadowUV = ((posForShadow.xy / posForShadow.w) * 0.5f) + 0.5f;
		shadowUV.y = 1.0f - shadowUV.y;

		float depthFromLight = posForShadow.z / posForShadow.w;

		shadowAmount = ShadowMap2.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
	}
	else if (vec.x >= -cascadeRange3.x / 2.001f && vec.x <= cascadeRange3.x / 2.001f && vec.y >= -cascadeRange3.y / 2.001f && vec.y <= cascadeRange3.y / 2.001f) {
		//return float4(1.0f, 0.0f, 1.0f, 1.0f);
		float4 posForShadow = mul(mul(float4(worldPos, 1.0f), shadowView), shadowProj3);
		float2 shadowUV = ((posForShadow.xy / posForShadow.w) * 0.5f) + 0.5f;
		shadowUV.y = 1.0f - shadowUV.y;

		float depthFromLight = posForShadow.z / posForShadow.w;

		shadowAmount = ShadowMap3.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);
	}

	float3 toCameraVector = normalize(cameraPos - input.worldPos);


	float3 finalColor = (surfaceColor * 0.1f) * shadowAmount + (surfaceColor * 0.01f);
	/*
	float3 finalColor = float3(0.f, 0.f, 0.f);
	for (int i = 0; i < lightCount; i++)
	{
		switch (lights[i].Type) {
		case LIGHT_TYPE_DIR:
			finalColor += (CalcDirectionalLight(surfaceColor, input.normal, lights[i], toCameraVector, specularValue, shadowAmount, brightness));
			break;
		case LIGHT_TYPE_POINT:
			finalColor += (CalcPointLight(surfaceColor, input.normal, lights[i], toCameraVector, specularValue, input.worldPos, brightness));
			break;
		case LIGHT_TYPE_SPOT:
			finalColor += (CalcSpotLight(surfaceColor, input.normal, lights[i], toCameraVector, specularValue, input.worldPos, brightness));
			break;
		}
	}
	*/

	float3 gammaCorrect = pow(abs(finalColor), 1.0f / 2.2f);

	return float4(gammaCorrect, 1.f);
}