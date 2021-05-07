#include "Lighting.hlsli"

#define MAX_LIGHTS 32
#define MAX_KERNEL_SAMPLES 32



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
	float4 posForShadow : SHADOW;
};

cbuffer lightCBuffer : register(b0)
{
	Light lights[MAX_LIGHTS];
	int lightCount;
};

cbuffer uvRepeatCBuffer : register(b1) {
	float2 uvMult;
};

cbuffer externalData : register(b2) {

	int specularValue;
	float brightness;
	float3 cameraPosition;
	int illumination;
	float3 manualColor;
	matrix ssaoView;
	matrix ssaoProjection;
};

cbuffer ssaoKernel : register(b3) {
	float4 kernel[MAX_KERNEL_SAMPLES];
	unsigned int sampleCount;
	float kernelRadius;
};

Texture2D DiffuseTexture  :  register(t0);

Texture2D ShadowMap		  : register(t1);

Texture2D DepthStencilMap : register(t2);

SamplerState BasicSampler               : register(s0);

SamplerComparisonState ShadowSampler	: register(s1);

SamplerState DepthStencilSampler	: register(s2);

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
	input.uv = float2(input.uv.x * uvMult.x, input.uv.y * uvMult.y);

	float4 surfaceColor = DiffuseTexture.Sample(BasicSampler, input.uv);
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering

	if (illumination == 11) {
		surfaceColor = surfaceColor.xyzw - (1 - float4(manualColor.xyz, 0));
	}

	// Shadow calculations
	float2 shadowUV = input.posForShadow.xy / input.posForShadow.w * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y;

	// This pixel's actual depth from the light
	float depthFromLight = input.posForShadow.z / input.posForShadow.w;

	// Sample the shadow map in the same location to get
	// the closest depth along that "ray" from the light
	// (Samples the shadow map with comparison built in)
	float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);

	//return ShadowMap.Sample(BasicSampler, shadowUV);

	float3 toCameraVector = normalize(cameraPosition - input.worldPos);


	float3 finalColor = float3(0.f,0.f,0.f);
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

	float ambientAccessibility = 1.0f;
	float ambientScalar = 1.0f;
	//float sampleWeight = 0.25f / sampleCount;
	float4 kernelSample;

	float2 pixelDepthStencilUV = input.position.xy / input.position.w * 0.5f + 0.5f;

	for (int i = 0; i < sampleCount; i++)
	{
		kernelSample = float4(kernel[i].xyz * kernelRadius, 1.0f);
		kernelSample = float4(input.worldPos.xyz + kernelSample.xyz, 1.0f);
		kernelSample = mul(mul(kernelSample, ssaoView), ssaoProjection);

		// SSAO calculations
		float2 ssaoUV = kernelSample.xy / kernelSample.w * 0.5f + 0.5f;
		ssaoUV.y = 1.0f - ssaoUV.y;

		// This sample point's actual depth
		float depth = kernelSample.z / kernelSample.w;

		// Sample the shadow map in the same location to get
		// the closest depth along that "ray" from the light
		// (Samples the shadow map with comparison built in)
		//float result = DepthStencilMap.SampleCmpLevelZero(DepthStencilSampler, ssaoUV, depth);

		//ambientAccessibility *= result;
	}

	if (ambientAccessibility < 0.25f) ambientAccessibility = 0.25f;

	//finalColor = finalColor.xyz * ambientAccessibility * ambientScalar;

	pixelDepthStencilUV.y = 1.0f - pixelDepthStencilUV.y;
	float4 depthBuffer = DepthStencilMap.Sample(DepthStencilSampler, pixelDepthStencilUV);
	finalColor = depthBuffer.xyz;

	//finalColor = kernel[31];

	//finalColor.xyz = ambientAccessibility / 4.0f;

	//float4 v = mul(float4(kernel[0].xyz, 1.0f), input.ssaoWVP);
	//finalColor = float3(v.z / v.w, v.z / v.w, v.z/ v.w);

	//finalColor = float3(input.position.z / input.position.w,input.position.z / input.position.w,input.position.z / input.position.w);

	//finalColor = float3(input.ssaoWVP.x, input.ssaoWVP.y, input.ssaoWVP.z);


	float3 gammaCorrect = pow(abs(finalColor), 1.0f / 2.2f);

	return float4(gammaCorrect, 1.f);
}