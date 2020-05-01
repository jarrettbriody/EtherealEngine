
// We want data from C++
cbuffer externalData : register(b0)
{
	float3 surfaceColor;
	float3 cameraPosition;

	float uvScale0;
	float uvScale1;
	float uvScale2;

	float3 dirLightDirection;
	float3 dirLightDiffuseColor;
	float3 dirLightAmbientColor;

	float3 pointLightPosition;
	float  pointLightRange;
	float3 pointLightDiffuseColor;
	float3 pointLightAmbientColor;
}


// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
};

// Textures and samplers
Texture2D blendMap			: register(t0);

Texture2D texture0			: register(t1);
Texture2D texture1			: register(t2);
Texture2D texture2			: register(t3);

Texture2D normalMap0		: register(t4);
Texture2D normalMap1		: register(t5);
Texture2D normalMap2		: register(t6);

SamplerState samplerOptions : register(s0);

// Range-based attenuation function
float Attenuate(float3 lightPos, float lightRange, float3 worldPos)
{
	float dist = distance(lightPos, worldPos);

	// Ranged-based attenuation
	float att = saturate(1.0f - (dist * dist / (lightRange * lightRange)));

	// Soft falloff
	return att * att;
}

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	
	// Re-normalize interpolated normals!
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);

	// Sample the texture, using the provided options, as
	// the specified UV coordinates
	float4 blend = blendMap.Sample(samplerOptions, input.uv);
	
	float4 color0 = texture0.Sample(samplerOptions, input.uv * uvScale0);
	float4 color1 = texture1.Sample(samplerOptions, input.uv * uvScale1);
	float4 color2 = texture2.Sample(samplerOptions, input.uv * uvScale2);

	float3 normalFromMap0 = normalMap0.Sample(samplerOptions, input.uv * uvScale0).rgb * 2 - 1;
	float3 normalFromMap1 = normalMap1.Sample(samplerOptions, input.uv * uvScale1).rgb * 2 - 1;
	float3 normalFromMap2 = normalMap2.Sample(samplerOptions, input.uv * uvScale2).rgb * 2 - 1;

	// Blend the textures together
	float4 textureColor =
		color0 * blend.r +
		color1 * blend.g +
		color2 * blend.b;

	float3 normalFromMap = 
		normalize(
			normalFromMap0 * blend.r +
			normalFromMap1 * blend.g +
			normalFromMap2 * blend.b);
	
	// === Normal mapping here!  We need a new normal for the rest of the lighting steps ===
	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N)); // Orthogonalize!
	float3 B = cross(T, N); // The bi-tangent

	float3x3 TBN = float3x3(T, B, N);

	// Rotate the normal from the normal map by our TBN rotation matrix
	input.normal = normalize(mul(normalFromMap, TBN));

	// Calculations that don't depend on individual lights
	float3 toCameraVector = normalize(cameraPosition - input.worldPos);


	// DIRECTIONAL LIGHT CALCULATION =====================

	// Diffuse calculation
	// Note: Remember to REVERSE the light's direction (need dir TO the light)
	// Note: Saturate() clamps values between 0 - 1
	// Note: Our light direction is IDEALLY normalized already, but just incase...
	float NdotL_dir = saturate(dot(input.normal, -normalize(dirLightDirection)));

	// Specular calculation (Blinn Phong)
	float3 halfwayVector_dir = normalize(-dirLightDirection + toCameraVector);
	float NdotH_dir = dot(input.normal, halfwayVector_dir);

	// Final color for directional light
	float3 finalColor_dir =
		dirLightAmbientColor * textureColor.rgb +
		dirLightDiffuseColor * textureColor.rgb * NdotL_dir;


	// POINT LIGHT CALCULATIONS =======================

	// Diffuse calculation
	float3 dirToPointLight = normalize(pointLightPosition - input.worldPos);
	float NdotL_point = saturate(dot(input.normal, dirToPointLight));

	// Specular calculation (Blinn Phong)
	float3 halfwayVector_point = normalize(dirToPointLight + toCameraVector);
	float NdotH_point = dot(input.normal, halfwayVector_point);

	float att = Attenuate(pointLightPosition, pointLightRange, input.worldPos);

	// Final color for directional light
	float3 finalColor_point =
		pointLightAmbientColor * textureColor.rgb +
		pointLightDiffuseColor * textureColor.rgb * NdotL_point * att;


	// FINAL COLOR COMBINE =============================
	return float4(finalColor_dir + finalColor_point, 1);
}