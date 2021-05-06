#pragma once
#include "pch.h"
#include "Config.h"
#include "EEString.h"

using namespace std;
using namespace DirectX;

//0. Color on and Ambient off
//1. Color on and Ambient on
//2. Highlight on
//3. Reflection on and Ray trace on
//4. Transparency: Glass on, Reflection : Ray trace on
//5. Reflection : Fresnel on and Ray trace on
//6. Transparency : Refraction on, Reflection : Fresnel off and Ray trace on
//7. Transparency : Refraction on, Reflection : Fresnel on and Ray trace on
//8. Reflection on and Ray trace off
//9. Transparency : Glass on, Reflection : Ray trace off
//10. Casts shadows onto invisible surfaces
//11. Custom color
struct MaterialData {
	XMFLOAT3 AmbientColor = XMFLOAT3(0.0f, 0.0f, 0.0f); //Ka
	XMFLOAT3 DiffuseColor = XMFLOAT3(0.0f, 0.0f, 0.0f); //Kd
	XMFLOAT3 SpecularColor = XMFLOAT3(0.0f, 0.0f, 0.0f); //Ks
	float SpecularExponent = 0.0f; //Ns (0-1000)
	float Transparency = 1.0f; //d where 1.0f is opaque or Tr where 0.0f is opaque
	int Illumination = 1; //illum where the value follows table above
	ID3D11ShaderResourceView* AmbientTextureMapSRV = nullptr; //map_Ka
	ID3D11ShaderResourceView* DiffuseTextureMapSRV = nullptr; //map_Kd
	ID3D11ShaderResourceView* SpecularColorTextureMapSRV = nullptr; //map_Ks
	ID3D11ShaderResourceView* SpecularHighlightTextureMapSRV = nullptr; //map_Ns
	ID3D11ShaderResourceView* AlphaTextureMapSRV = nullptr; //map_d
	ID3D11ShaderResourceView* NormalTextureMapSRV = nullptr; //map_Bump
	bool SSAO = false;
	bool hbaoPlusEnabled = true;
	XMFLOAT2 repeatTexture = XMFLOAT2(1.0f, 1.0f);
	XMFLOAT2 uvOffset = XMFLOAT2(0.0f, 0.0f);
};

class Material
{
protected:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	MaterialData materialData;
	ID3D11SamplerState* samplerState;
	EEString<EESTRING_SIZE> name;
public:
	Material();
	Material(string n, MaterialData matData, SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11SamplerState* sampler);
	~Material();
	void operator= (const Material& m);
	void FreeMemory();
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	MaterialData GetMaterialData();
	ID3D11SamplerState* GetSamplerState();
	void Prepare();
	string GetName();
};

