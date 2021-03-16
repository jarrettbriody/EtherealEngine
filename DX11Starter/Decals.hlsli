
struct Decal {
	matrix localTransform;
	int type;
};

float2 GetDecalUV(Decal d, float3 worldPos, float3 minPoint, matrix worldMatrix)
{

	float3 decalUVec = mul(mul(float4(0.5f, -0.5f, -0.5f, 1.0f), d.localTransform), worldMatrix).xyz - minPoint;
	float3 decalVVec = mul(mul(float4(-0.5f, 0.5f, -0.5f, 1.0f), d.localTransform), worldMatrix).xyz - minPoint;
	float3 relativePixelPos;
	float decalUVecLength;
	float decalVVecLength;
	float2 decalUV;
	decalUVecLength = length(decalUVec);
	decalVVecLength = length(decalVVec);
	relativePixelPos = worldPos - minPoint;
	decalUV.x = length(dot(relativePixelPos, decalUVec) / (decalUVecLength * decalUVecLength)) / decalUVecLength;
	decalUV.y = 1 - length(dot(relativePixelPos, decalVVec) / (decalVVecLength * decalVVecLength)) / decalVVecLength;
	return decalUV;
	//surfaceColor = Decals.Sample(BasicSampler, decalUV);
	//if (surfaceColor.a < 1.0f) continue;
	//surfaceColor = Decals[d.type].Sample(BasicSampler, decalUV);
}