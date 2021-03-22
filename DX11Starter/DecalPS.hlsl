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
	int specularValue;
	float3 manualColor;
	matrix worldMatrix;
	matrix inverseWorldMatrix;
};

cbuffer constantPerFrame : register(b2) {
	float3 cameraPos;
	matrix shadowViewProj;
	matrix shadowView;
	matrix shadowProj;
};

Texture2D DepthBuffer	  : register(t0);
Texture2D ShadowMap		  : register(t1);
Texture2D Decal			  :  register(t2);

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
	float  depth = DepthBuffer.Load(pixelIndex).r;

	input.viewRay = normalize(input.viewRay);

	float3 worldPos = cameraPos + input.viewRay * depth;

	float3 objPos = mul(float4(worldPos, 1.0f), inverseWorldMatrix);

	clip(0.5f - abs(objPos.xyz));

	//return float4(1.0f, 0.0f, 0.0f, 1.0f);

	float2 decalUV = objPos.xy + 0.5f;

	float4 surfaceColor = Decal.Sample(BasicSampler, decalUV);

	//float3 decalUV = float3((pos.xy + 0.5f) * 512, 0);

	//float4 surfaceColor = Decal.Load(decalUV);

	if (surfaceColor.a < 0.1f) discard;

	if (illumination == 11) {
		surfaceColor = surfaceColor.xyzw - (1 - float4(manualColor.xyz, 0));
	}

	// Shadow calculations

	// Calculate shadow map position
	float4 posForShadow = mul(mul(float4(worldPos, 1.0f), shadowView), shadowProj);
	float2 shadowUV = ((posForShadow.xy / posForShadow.w) * 0.5f) + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y;

	// This pixel's actual depth from the light
	float depthFromLight = posForShadow.z / posForShadow.w;

	// Sample the shadow map in the same location to get
	// the closest depth along that "ray" from the light
	// (Samples the shadow map with comparison built in)

	float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);

	float3 toCameraVector = normalize(cameraPos - input.worldPos);


	float3 finalColor = (surfaceColor * 0.1f) * shadowAmount + (surfaceColor * 0.01f);;
	/*
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
	*/

	float3 gammaCorrect = pow(abs(finalColor), 1.0f / 2.2f);

	return float4(gammaCorrect, 1.f);
}