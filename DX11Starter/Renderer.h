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
	SimplePixelShader* depthStencilPS = nullptr;
	SimpleVertexShader* debugLineVS = nullptr;
	SimplePixelShader* debugLinePS = nullptr;
	SimpleVertexShader* decalVS = nullptr;
	SimplePixelShader* decalPS = nullptr;
	SimpleVertexShader* skyVS = nullptr;
	SimplePixelShader* skyPS = nullptr;
};

struct RendererCallback {
	void* data = nullptr;
	bool active = false;

	SimpleVertexShader* vShader = nullptr;
	SimplePixelShader* pShader = nullptr;

	SimpleVertexShader* prepassVShader = nullptr;
	SimplePixelShader* prepassPShader = nullptr;

	virtual void PreVertexShaderCallback() {};
	virtual void PrePixelShaderCallback() {};
	virtual void PrePrepassVertexShaderCallback() {};
	virtual void PrePrepassPixelShaderCallback() {};
};

struct RenderObject{
	Entity* entity;
	Mesh* mesh;
	Material* material;
	RendererCallback* callback;
};

struct ShadowComponents {
	unsigned int shadowMapResolution = 2048;
	ID3D11DepthStencilView* shadowDSV = nullptr;
	ID3D11ShaderResourceView* shadowSRV = nullptr;
	ID3D11SamplerState* shadowSampler = nullptr;
	ID3D11RasterizerState* shadowRasterizer = nullptr;

	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
	DirectX::XMFLOAT4X4 shadowViewProj;
};

struct DepthStencilComponents {
	ID3D11DepthStencilView* depthStencilDSV = nullptr;
	ID3D11RenderTargetView* depthStencilRTV = nullptr;
	ID3D11ShaderResourceView* depthStencilSRV = nullptr;
	ID3D11SamplerState* depthStencilSampler = nullptr;
	ID3D11RasterizerState* depthStencilRasterizer = nullptr;
	ID3D11DepthStencilState* depthStencilState = nullptr;

	ID3D11RenderTargetView* entityInfoRTV = nullptr;
	ID3D11ShaderResourceView* entityInfoSRV = nullptr;

	ID3D11BlendState* decalBlendState = nullptr;
};

struct HBAOPlusComponents {
	GFSDK_SSAO_CustomHeap CustomHeap;
	GFSDK_SSAO_Status status;
	GFSDK_SSAO_Context_D3D11* pAOContext = nullptr;
	GFSDK_SSAO_InputData_D3D11 Input;
	GFSDK_SSAO_Parameters_D3D11 Params;
};

struct SkyboxComponents {
	ID3D11ShaderResourceView* skySRV = nullptr;
	ID3D11RasterizerState* skyRasterizer = nullptr;
	ID3D11DepthStencilState* skyDepthStencilState = nullptr;
};

class Renderer
{
private:
	static Renderer* instance;

	vector<Entity*>* entities = nullptr;
	RenderObject* renderObjects;
	int renderObjectCount = 0;
	int maxRenderObjects = 0;
	map<Entity*, vector<RenderObject*>> renderObjectsMap;

	RendererShaders shaders;

	Mesh* cube = nullptr;
	Mesh* invCube = nullptr;

	Camera* camera = nullptr;
	map<string, Camera*> cameras;
	unsigned int cameraCount = 0;

	map<string, Light*> lights;
	//map<string, Shadow> shadows;
	unsigned int lightCount = 0;

	ShadowComponents shadowComponents;
	DepthStencilComponents depthStencilComponents;
	HBAOPlusComponents hbaoPlusComponents;
	SkyboxComponents skyboxComponents;

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
	void SetMeshes(Mesh* cube, Mesh* invCube);

	void InitDepthStencil();
	void InitHBAOPlus();
	void InitShadows();
	void InitSkybox();
	void SetSkybox(ID3D11ShaderResourceView* srv);
	void SetShadowMapResolution(unsigned int res);

	void ClearFrame();
	void RenderFrame();
	void PresentFrame();
	void RenderDebugLines();
	void RenderShadowMap();
	void RenderDepthStencil();
	void RenderSkybox();

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

	void SetRenderObjectCallback(Entity* e, RendererCallback* callback);
};
