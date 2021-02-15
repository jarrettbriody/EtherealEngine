#include "TerrainMaterial.h"

TerrainMaterial::TerrainMaterial(string n, TerrainMaterialData matData, SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11SamplerState* sampler) : Material()
{
	vertexShader = vShader;
	pixelShader = pShader;
	terrainMaterialData = matData;
	samplerState = sampler;
	name = new string(n);
}

TerrainMaterial::~TerrainMaterial()
{
	vertexShader = nullptr;
	pixelShader = nullptr;
	samplerState = nullptr; 
	if (name != nullptr)
		delete name;
}

void TerrainMaterial::Prepare()
{
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	vertexShader->SetShader();
	pixelShader->SetShader();

	pixelShader->SetSamplerState("BasicSampler", samplerState);

	pixelShader->SetShaderResourceView("SurfaceTexture1", terrainMaterialData.SurfaceTexture1);
	pixelShader->SetShaderResourceView("SurfaceTexture2", terrainMaterialData.SurfaceTexture2);
	pixelShader->SetShaderResourceView("SurfaceTexture3", terrainMaterialData.SurfaceTexture3);

	pixelShader->SetShaderResourceView("SurfaceNormal1", terrainMaterialData.SurfaceNormal1);
	pixelShader->SetShaderResourceView("SurfaceNormal2", terrainMaterialData.SurfaceNormal2);
	pixelShader->SetShaderResourceView("SurfaceNormal3", terrainMaterialData.SurfaceNormal3);

	pixelShader->SetShaderResourceView("BlendMap", terrainMaterialData.BlendMap);

	pixelShader->SetFloat("uvScale", terrainMaterialData.uvScale);

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