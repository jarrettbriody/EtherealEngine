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
	float3 cameraPosition;
	int textureCount;
	float uvScale;
}

Texture2D SurfaceTexture1  :  register(t0);
Texture2D SurfaceTexture2  :  register(t1);
Texture2D SurfaceTexture3  :  register(t2);

Texture2D SurfaceNormal1  :  register(t0);
Texture2D SurfaceNormal2  :  register(t1);
Texture2D SurfaceNormal3  :  register(t2);

Texture2D BlendMap			:  register(t3);

Texture2D ShadowMap		  : register(t4);

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
	input.uv = float2(input.uv.x * uvMult.x, input.uv.y * uvMult.y);

	float4 surfaceColor1 = SurfaceTexture1.Sample(BasicSampler, input.uv * uvScale);
	float4 surfaceColor2 = SurfaceTexture2.Sample(BasicSampler, input.uv * uvScale);
	float4 surfaceColor3 = SurfaceTexture3.Sample(BasicSampler, input.uv * uvScale);

	float4 blend = BlendMap.Sample(BasicSampler, input.uv);


	float4 surfaceColor =
		surfaceColor1 * blend.r +
		surfaceColor2 * blend.g +
		surfaceColor3 * blend.b;

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering

	float3 normal1 = SurfaceNormal1.Sample(BasicSampler, input.uv).rgb * 2 - 1;
	float3 normal2 = SurfaceNormal2.Sample(BasicSampler, input.uv).rgb * 2 - 1;
	float3 normal3 = SurfaceNormal3.Sample(BasicSampler, input.uv).rgb * 2 - 1;


	float3 normalFromMap = normalize(
		normal1 * blend.r +
		normal2 * blend.g + 
		normal3 * blend.b
	);

	//normals
	// Calculate the matrix we'll use to convert from tangent to world space
	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);

	// Use the normal from the map, after we've converted it to world space
	input.normal = normalize(mul(normalFromMap, TBN));

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

	return surfaceColor;

	float3 finalColor = float3(0.f,0.f,0.f);
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

	return float4(gammaCorrect, 1.f);
}