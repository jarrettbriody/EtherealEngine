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
	float uvScale;

	float normalOffsetX1;
	float normalOffsetY1;

	float normalOffsetX2;
	float normalOffsetY2;
}

Texture2D SurfaceTexture1  :  register(t0);
Texture2D SurfaceTexture2  :  register(t1);

Texture2D SurfaceNormal1  :  register(t3);
Texture2D SurfaceNormal2  :  register(t4);

Texture2D BlendMap			:  register(t6);

Texture2D ShadowMap		  : register(t7);

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

	float4 surfaceColor1 = SurfaceTexture1.Sample(BasicSampler, float2(input.uv * uvScale) + float2(normalOffsetX1, normalOffsetY1));
	float4 surfaceColor2 = SurfaceTexture2.Sample(BasicSampler, float2(input.uv * uvScale) + float2(normalOffsetX2, normalOffsetY2));


	float4 surfaceColor = surfaceColor1;

	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering

		
	float3 normal1 = SurfaceNormal1.Sample(BasicSampler, float2(input.uv) + float2(normalOffsetX1, normalOffsetY1)).rgb * 2 - 1;
	float3 normal2 = SurfaceNormal2.Sample(BasicSampler, float2(input.uv) + float2(normalOffsetX2, normalOffsetY2)).rgb * 2 - 1;


	float3 normalFromMap = normalize(
		normal1 +
		normal2 
	);


	//normals
	// Calculate the matrix we'll use to convert from tangent to world space
	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);

	// Use the normal from the map, after we've converted it to world space
	input.normal = normalize(mul(normalFromMap, TBN));
	
	float3 toCameraVector = normalize(cameraPosition - input.worldPos);

	float3 light_dir = -normalize(lights[0].Direction);
	float lightAmount = saturate(dot(input.normal, light_dir));

	//specular
	float3 halfwayVector_dir = normalize(light_dir + toCameraVector);
	float NdotH_dir = dot(input.normal, halfwayVector_dir);
	float specLight = saturate(pow(NdotH_dir, 100.f));


	float3 finalColor = lights[0].Color * surfaceColor +
		lights[0].Color * surfaceColor * lightAmount +
		specLight;

	

	return float4(finalColor, 1.f);
}

