#pragma once
#include "SimpleShader.h"
class Material
{
private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11SamplerState* samplerState;
public:
	Material();
	Material(SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11ShaderResourceView* shaderRV, ID3D11SamplerState* sampler);
	~Material();
	SimpleVertexShader* GetVertexShader(); 
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11SamplerState* GetSamplerState();
};

