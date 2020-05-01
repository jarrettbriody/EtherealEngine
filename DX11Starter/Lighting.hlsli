#define LIGHT_TYPE_DIR 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

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

//utility methods

float GetSpecularity(float3 normal, float3 negatedLightDir, float3 toCameraVector, float specularValue) 
{
	float3 halfwayVector = normalize(negatedLightDir + toCameraVector);
	return pow(max(dot(halfwayVector, normal), 0), specularValue);
}

float Attenuate(Light light, float3 worldPos) 
{
	float dist = distance(light.Position, worldPos);

	float attenuation = saturate(1.0f - (dist * dist / (light.Range * light.Range)));

	return attenuation * attenuation;
}

//Lighting methods

float3 CalcDirectionalLight(float4 surfaceColor, float3 normal, Light light, float3 toCameraVector, float specularValue, float shadowAmount) 
{
	float3 toLight = normalize(-light.Direction);
	float diffuse = saturate(dot(normal, toLight));
	
	float3 finalColor = diffuse * surfaceColor;

	if (specularValue > 0)
	{
		finalColor += GetSpecularity(normal, toLight, toCameraVector, specularValue);
	}

	finalColor *= light.Intensity * light.Color;

	return (finalColor * shadowAmount) + (surfaceColor * 0.05f);
}

float3 CalcPointLight(float4 surfaceColor, float normal, Light light, float3 toCameraVector, float specularValue, float3 worldPos) 
{
	float3 toLight = normalize(light.Position - worldPos);

	float atten = Attenuate(light, worldPos);
	float diffuse = saturate(dot(normal, toLight));
	
	float3 finalColor = diffuse * surfaceColor;

	if (specularValue > 0)
	{
		//finalColor += GetSpecularity(normal, toLight, toCameraVector, specularValue);
	}
	finalColor *= atten * light.Intensity * light.Color;

	
	return finalColor + (surfaceColor * 0.05f);
}

float3 CalcSpotLight(float4 surfaceColor, float normal, Light light, float3 toCameraVector, float specularValue, float3 worldPos)
{
	float3 toLight = normalize(light.Position - worldPos);
	float penumbra = pow(saturate(dot(-toLight, light.Direction)), light.SpotFalloff);

	return CalcPointLight(surfaceColor, normal, light, toCameraVector, specularValue, worldPos) * penumbra;
}