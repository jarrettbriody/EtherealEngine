#pragma once
#include "pch.h"
#include "Vertex.h"
#include "Lights.h"
#include "SimpleShader.h"
#include "Entity.h"
#include "Camera.h"
#include "DebugLines.h"
#include "DecalHandler.h"

using namespace std;

#define MAX_LIGHTS 32

struct RendererShaders {
	SimpleVertexShader* depthStencilVS = nullptr;
	SimpleVertexShader* debugLineVS = nullptr;
	SimplePixelShader* debugLinePS = nullptr;
};

struct RenderObject{
	Entity* entity;
	Mesh* mesh;
	Material* material;
};

struct ShadowComponents {
	unsigned int shadowMapResolution = 2048;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11SamplerState* shadowSampler;
	ID3D11RasterizerState* shadowRasterizer;

	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
};

struct DepthStencilComponents {
	ID3D11DepthStencilView* depthStencilDSV;
	ID3D11ShaderResourceView* depthStencilSRV;
	ID3D11SamplerState* depthStencilSampler;
	ID3D11RasterizerState* depthStencilRasterizer;
};

struct HBAOPlusComponents {
	GFSDK_SSAO_CustomHeap CustomHeap;
	GFSDK_SSAO_Status status;
	GFSDK_SSAO_Context_D3D11* pAOContext;
	GFSDK_SSAO_InputData_D3D11 Input;
	GFSDK_SSAO_Parameters_D3D11 Params;
};

class Renderer
{
private:
	static Renderer* instance;

	vector<Entity*>* entities = nullptr;
	RenderObject* renderObjects;
	int renderObjectCount = 0;
	int maxRenderObjects = 0;

	RendererShaders shaders;

	Camera* camera = nullptr;
	map<string, Camera*> cameras;
	unsigned int cameraCount = 0;

	map<string, Light*> lights;
	//map<string, Shadow> shadows;
	unsigned int lightCount = 0;

	ShadowComponents shadowComponents;
	DepthStencilComponents depthStencilComponents;
	HBAOPlusComponents hbaoPlusComponents;

	ID3D11ShaderResourceView* decals[8];

	Renderer();
	~Renderer();
public:
	static bool SetupInstance();
	static Renderer* GetInstance();
	static bool DestroyInstance();

	void SetEntities(vector<Entity*>* entities);
	void SetRendererShaders(RendererShaders rShaders);
	void SetDecals(ID3D11ShaderResourceView* decals[8]);

	void InitDepthStencil();

	void InitHBAOPlus();
	
	void InitShadows();
	void SetShadowMapResolution(unsigned int res);

	void ClearFrame();
	void RenderFrame();
	void PresentFrame();
	void RenderDebugLines();
	void RenderShadowMap();
	void RenderDepthStencil();

	bool AddCamera(string name, Camera* newCamera);
	bool RemoveCamera(string name);
	Camera* GetCamera(string name);
	bool EnableCamera(string name);

	bool AddLight(std::string name, Light* newLight);
	bool RemoveLight(std::string name);
	void SendAllLightsToShader(SimplePixelShader* pixelShader);
	Light* GetLight(string name);

	void SendSSAOKernelToShader(SimplePixelShader* pixelShader);

	void AddRenderObject(Entity* e, Mesh* mesh, Material* mat = nullptr);
};
