#include "Material.h"

Material::Material()
{
	vertexShader = 0;
	pixelShader = 0;
}

Material::Material(SimpleVertexShader * vShader, SimplePixelShader * pShader, ID3D11ShaderResourceView* shaderRV, ID3D11SamplerState* sampler)
{
	vertexShader = vShader;
	pixelShader = pShader;
	shaderResourceView = shaderRV;
	samplerState = sampler;
}

Material::~Material()
{
	shaderResourceView->Release();
	samplerState->Release();
}

SimpleVertexShader * Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Material::GetShaderResourceView()
{
	return shaderResourceView;
}

ID3D11SamplerState * Material::GetSamplerState()
{
	return samplerState;
}
