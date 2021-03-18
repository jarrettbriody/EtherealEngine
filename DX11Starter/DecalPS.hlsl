//#include "Lighting.hlsli"

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
	float4 positionViewSpace : VIEWSPACEPOS;
};

/*
cbuffer lightCBuffer : register(b0)
{
	Light lights[MAX_LIGHTS];
	int lightCount;
};
*/

cbuffer externalData : register(b1) {

	int specularValue;
	float3 cameraPosition;
	float farClip;
	float3 decalPosition;
}

cbuffer matrices : register(b2) {
	//matrix ownerWorld;
	//matrix localWorld;
	matrix inverseWorldMatrix;
	matrix inverseOwnerWorld;
	matrix inverseLocalWorld;
	matrix inverseView;
	matrix projection;
}

Texture2D Decals		  :  register(t0);

Texture2D DepthBuffer	  : register(t1);

Texture2D ShadowMap		  : register(t2);

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
	//return float4(1.0f, 0.0f, 0.0f, 1.0f);

	//return float4(input.position.xyz / input.position.w,1.0f);

	float2 screenPosition = input.position.xy / input.position.w;

	float2 depthBufferUV = screenPosition * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	depthBufferUV += float2(0.5f, 0.5f);

	//Sample a value from the depth buffer
	float depth = DepthBuffer.Load(int3(depthBufferUV,0)).r;

	float2 deproject = float2(projection._11, projection._22);
	float4 scenePosView = float4(input.position.xy * depth / (deproject.xy * input.position.w), -depth, 1);

	float3 pos = mul(scenePosView, inverseWorldMatrix);
	clip(0.5f - abs(pos.xyz));

	return float4(1.0f, 0.0f, 0.0f, 1.0f);

	/*

	//convert to UV so we can sample depth buffer
	//float2 depthBufferUV = screenPosition * 0.5f + 0.5f;
	//depthBufferUV.y = 1.0f - depthBufferUV.y;
	//Convert into a texture coordinate
	float2 depthBufferUV = float2(
		(1 + screenPosition.x) / 2 + (0.5 / 1600),
		(1 - screenPosition.y) / 2 + (0.5 / 900)
		);

	//float4 depthSample = DepthBuffer.Sample(ShadowSampler, depthBufferUV);

	//return float4(depthSample.xxx, 1.0f);

	float3 viewRay = normalize(input.positionViewSpace.xyz * (farClip / -input.positionViewSpace.z));

	float3 viewPosition = viewRay.xyz * depthSample.r;

	float3 worldPosOfPixelOnWall = mul(float4(viewPosition, 1), inverseView).xyz;

	//return float4(viewPosition, 1.0f);

	//return float4(worldPosOfPixelOnWall.xyz, 1.0f);

	//matrix actualInverseWorld = mul(inverseLocalWorld, inverseOwnerWorld);

	float4 objectPosOfPixelOnWall = mul(float4(worldPosOfPixelOnWall, 1), inverseWorldMatrix);

	//float4 objectPosOfPixelOnWall = mul(float4(input.worldPos, 1), inverseWorldMatrix);

	//objectPosOfPixelOnWall = mul(objectPosOfPixelOnWall, inverseLocalWorld);

	//return objectPosOfPixelOnWall;

	clip(0.5f - abs(objectPosOfPixelOnWall.xyz));
	//clip(1.0f - abs(objectPosOfPixelOnWall.y));
	//clip(1.0f - abs(objectPosOfPixelOnWall.z));

	return float4(1.0f, 0.0f, 0.0f, 1.0f);
	*/

	/*
	input.uv = float2(input.uv.x * uvMult.x, input.uv.y * uvMult.y);

	float4 surfaceColor = DiffuseTexture.Sample(BasicSampler, input.uv);
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering

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
	*/
}