#include "WaterMaterial.h"

WaterMaterial::WaterMaterial(string n, WaterMaterialData matData, SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11SamplerState* sampler) : Material()
{
	vertexShader = vShader;
	pixelShader = pShader;
	materialData = matData;
	samplerState = sampler;
	name = new string(n);
}

WaterMaterial::~WaterMaterial()
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	samplerState = nullptr;
	if (name != nullptr)
		delete name;
}

void WaterMaterial::Prepare()
{
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	vertexShader->SetShader();
	pixelShader->SetShader();

	pixelShader->SetSamplerState("BasicSampler", samplerState);

	pixelShader->SetShaderResourceView("SurfaceTexture1", materialData.SurfaceTexture1);
	pixelShader->SetShaderResourceView("SurfaceTexture2", materialData.SurfaceTexture2);

	pixelShader->SetShaderResourceView("SurfaceNormal1", materialData.SurfaceNormal1);
	pixelShader->SetShaderResourceView("SurfaceNormal2", materialData.SurfaceNormal2);
	
	pixelShader->SetFloat("uvScale", materialData.uvScale);

	/*if (materialData.NormalTextureMapSRV) {
		pixelShader->SetShaderResourceView("NormalTexture", materialData.NormalTextureMapSRV);
	}*/

	/*if (materialData.SpecularExponent != 0)
	{
		pixelShader->SetInt("specularValue", materialData.SpecularExponent);
	}*/
	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	vertexShader->CopyAllBufferData();
	pixelShader->CopyAllBufferData();
}