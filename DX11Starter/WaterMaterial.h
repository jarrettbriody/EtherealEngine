#pragma once
#include "SimpleShader.h"
#include <string>
#include "Material.h"
#define MAX_TEXTURE_LAYERS 10

using namespace std;
using namespace DirectX;

struct WaterMaterialData {
	XMFLOAT3 AmbientColor = XMFLOAT3(0.0f, 0.0f, 0.0f); //Ka
	XMFLOAT3 DiffuseColor = XMFLOAT3(0.0f, 0.0f, 0.0f); //Kd
	XMFLOAT3 SpecularColor = XMFLOAT3(0.0f, 0.0f, 0.0f); //Ks
	float SpecularExponent = 0.0f; //Ns (0-1000)
	float Transparency = 1.0f; //d where 1.0f is opaque or Tr where 0.0f is opaque
	int Illumination = 1; //illum where the value follows table above
	ID3D11ShaderResourceView* SurfaceTexture1;
	ID3D11ShaderResourceView* SurfaceTexture2;
	ID3D11ShaderResourceView* SurfaceNormal1;
	ID3D11ShaderResourceView* SurfaceNormal2;
	float uvScale;
	vector<string> SRVNames;
};

class WaterMaterial : public Material
{
private:
	WaterMaterialData materialData;
public:
	WaterMaterial(string n, WaterMaterialData matData, SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11SamplerState* sampler);
	~WaterMaterial();
	void Prepare();
};

