#include "Material.h"

Material::Material(string n, MaterialData matData, SimpleVertexShader * vShader, SimplePixelShader * pShader, ID3D11SamplerState* sampler)
{
	vertexShader = vShader;
	pixelShader = pShader;
	materialData = matData;
	samplerState = sampler;
	name = n;
}

Material::~Material()
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	samplerState = nullptr;
}

SimpleVertexShader * Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Material::GetPixelShader()
{
	return pixelShader;
}

MaterialData Material::GetMaterialData()
{
	return materialData;
}

ID3D11SamplerState * Material::GetSamplerState()
{
	return samplerState;
}

string Material::GetName()
{
	return name;
}
