#define MAX_LIGHTS 32
#define LIGHT_TYPE_DIR 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2


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
	float3 view			: POSITION0;
	float3 worldPos		: POSITION1;
};

struct Light {
	int Type;
	float3 Direction;
	float Range;
	float3 Position;
	float Intensity;
	float3 Color;
	float SpotFalloff;
	float3 Padding;
};

cbuffer lightCBuffer : register(b0)
{
	Light lights[MAX_LIGHTS];
	int lightCount;
};

Texture2D DiffuseTexture  :  register(t0);

SamplerState BasicSampler :  register(s0);

float4 CalcDirectionalLighting(float3 n, Light l) {
	n = normalize(n);
	float3 negatedLightDir = -l.Direction;
	float lightAmt = saturate(dot(n, negatedLightDir));
	return (float4(l.Color,1.0f) * lightAmt + float4(0.1f,0.1f,0.1f,1.0f));
}

float4 CalcSpecular(float4 p, float3 v, float3 n, Light l) {
	float3 worldP = p.xyz;
	float3 dirToLight = normalize(v - l.Position);
	float3 dirToCam = normalize(worldP - l.Position);
	float3 h = normalize(dirToLight + dirToCam);
	float NdotH = saturate(dot(n, h));
	float specAmount = pow(NdotH, 64.0f);
	return specAmount;
}

float4 PointLight(float4 p, float3 n, Light l) {
	float3 worldP = p.xyz;
	float3 lightDir = normalize(worldP - l.Position);
	n = normalize(n);
	float NdotL = dot(n, -lightDir);
	return NdotL;
}

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
	float4 surfaceColor = DiffuseTexture.Sample(BasicSampler, input.uv);
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	float4 finalColor = float4(0,0,0,1);
	for (int i = 0; i < lightCount; i++)
	{
		switch (lights[i].Type) {
		case LIGHT_TYPE_DIR:
			finalColor += (CalcDirectionalLighting(input.normal, lights[i]) + CalcSpecular(input.position,input.view,input.normal,lights[i]))* surfaceColor;
			break;
		case LIGHT_TYPE_POINT:
			finalColor += (PointLight(input.position, input.normal, lights[i]) + CalcSpecular(input.position, input.view, input.normal, lights[i]))* surfaceColor;
			break;
		}
	}
	return finalColor;
}