#include "Material.h"

Material::Material()
{
}

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

void Material::Prepare()
{
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	vertexShader->SetShader();
	pixelShader->SetShader();

	pixelShader->SetSamplerState("BasicSampler", samplerState);
	pixelShader->SetShaderResourceView("DiffuseTexture", materialData.DiffuseTextureMapSRV);

	if (materialData.NormalTextureMapSRV) {
		pixelShader->SetShaderResourceView("NormalTexture", materialData.NormalTextureMapSRV);
	}

	if (materialData.SpecularExponent != 0)
	{
		pixelShader->SetInt("specularValue", materialData.SpecularExponent);
	}
	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	vertexShader->CopyAllBufferData();
	pixelShader->CopyAllBufferData();
}

string Material::GetName()
{
	return name;
}
