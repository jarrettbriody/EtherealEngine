#include "pch.h"
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

void Material::operator=(const Material& m)
{
	this->name = m.name;
	this->vertexShader = m.vertexShader;
	this->pixelShader = m.pixelShader;
	this->materialData = m.materialData;
	this->samplerState = m.samplerState;
}

void Material::FreeMemory()
{
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
	/*
	static SimpleVertexShader* lastVertShader = nullptr;
	static SimplePixelShader* lastPixelShader = nullptr;
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	if (vertexShader != lastVertShader) {
		vertexShader->SetShader();
		lastVertShader = vertexShader;
	}
	if (pixelShader != lastPixelShader) {
		pixelShader->SetShader();
		lastPixelShader = pixelShader;
	}
	*/
	vertexShader->SetShader();
	pixelShader->SetShader();

	pixelShader->SetSamplerState("BasicSampler", samplerState);
	pixelShader->SetShaderResourceView("DiffuseTexture", materialData.DiffuseTextureMapSRV);

	if (materialData.NormalTextureMapSRV) {
		pixelShader->SetShaderResourceView("NormalTexture", materialData.NormalTextureMapSRV);
	}

	pixelShader->SetInt("specularValue", materialData.SpecularExponent);
		
	pixelShader->SetInt("illumination", materialData.Illumination);

	pixelShader->SetFloat("brightness", Config::SceneBrightness * Config::SceneBrightnessMult);

	if (pixelShader->GetShaderType() == ShaderType::DEFAULT)
		pixelShader->SetFloat3("manualColor", materialData.DiffuseColor);

	pixelShader->SetFloat("transparency", materialData.Transparency);

	if (materialData.repeatTexture.x != 1.0f && materialData.repeatTexture.y != 1.0f) {
		pixelShader->SetData(
			"uvMult",
			&materialData.repeatTexture,
			sizeof(materialData.repeatTexture)
		);
	}
	pixelShader->SetData(
		"uvOffset",
		&materialData.uvOffset,
		sizeof(materialData.uvOffset)
	);

	/*
	if (materialData.SSAO) {
		pixelShader->SetData(
			"kernel",
			&Config::SSAOKernel[0],
			sizeof(XMFLOAT4) * Config::SSAOSampleCount
		);
		pixelShader->SetData(
			"sampleCount",
			&Config::SSAOSampleCount,
			sizeof(Config::SSAOSampleCount)
		);
		pixelShader->SetData(
			"kernelRadius",
			&Config::SSAOKernelRadius,
			sizeof(Config::SSAOKernelRadius)
		);
	}
	*/

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	vertexShader->CopyAllBufferData();
	pixelShader->CopyAllBufferData();
}

string Material::GetName()
{
	return name.STDStr();
}
