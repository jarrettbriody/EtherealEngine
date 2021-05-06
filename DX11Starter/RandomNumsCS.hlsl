
cbuffer ExternalData : register(b0)
{
	float seed1;
	float seed2;
	unsigned int count;
}

AppendStructuredBuffer<float> RandNums : register(u0);

float rand(float2 uv)
{
	float2 noise = (frac(sin(dot(uv, float2(12.9898f, 78.233f) * 2.0f)) * 43758.5453f));
	return abs(noise.x + noise.y) * 0.5f;
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	if ((id.x + 1) * (id.y + 1) > count) return;

	RandNums.Append(rand(float2((float)id.x * seed1, (float)id.x * seed2)));
}