
cbuffer Data : register(b0)
{
	float pixelWidth;
	float pixelHeight;
	int sampleSize;
	unsigned int outlineLayerMask;
}

struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

Texture2D Pixels		: register(t0);
Texture2D<uint> LayerMap		: register(t1);
//Texture2D<float> DepthMap		: register(t2);
SamplerState Sampler	: register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
	float3 pixelIndex = float3(input.position.xy, 0);
	float4 color = float4(1,1,1,1);
	float4 originalColor;
	float blackAmount = 0;
	uint numSamples = 0;
	uint samplesNotInLayer = 0;
	uint samplesInLayer = 0;

	for (int y = -sampleSize; y <= sampleSize; y += 1)
	{
		for (int x = -sampleSize; x <= sampleSize; x += 1)
		{
			float2 uv = input.uv + float2(x * pixelWidth, y * pixelHeight);
			float3 pixelIndexOffset = float3(pixelIndex.xy + float2(x, y), 0);
			uint layer = LayerMap.Load(pixelIndexOffset).r;
			//if(layer >)
			if ((outlineLayerMask | layer) == layer) samplesInLayer++;
			else samplesNotInLayer++;
			numSamples++;
		}
	}

	originalColor = Pixels.Sample(Sampler, input.uv);

	blackAmount = (samplesInLayer < samplesNotInLayer) ? ((float)samplesInLayer / (float)numSamples) : ((float)samplesNotInLayer / (float)numSamples);
	blackAmount *= 0.2f;

	//if (samplesInLayer > 0 && samplesInLayer != numSamples) {
	//	color.rgba *= (samplesInLayer < samplesNotInLayer) ? (1.0f - samplesInLayer / numSamples) : (1.0f - samplesNotInLayer / numSamples);
	//}
	//if (samplesInLayer == samplesNotInLayer) color = float4(0, 0, 0, 1.0f);

	//bool isBlack = (samplesInLayer > 0 && samplesInLayer != numSamples);
	//return float4(originalColor.xyz * (1 - (int)isBlack), 1.0f);
	return float4(originalColor.r - blackAmount, originalColor.g - blackAmount, originalColor.b - blackAmount, originalColor.a - originalColor.a * blackAmount);
}