#pragma once
#include "SimpleShader.h"
#include <string>

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
	vector<string> SRVNames;
};

class Material
{
private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	MaterialData materialData;
	ID3D11SamplerState* samplerState;
	string name;
public:
	Material(string n, MaterialData matData, SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11SamplerState* sampler);
	~Material();
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	MaterialData GetMaterialData();
	ID3D11SamplerState* GetSamplerState();
	string GetName();
};

