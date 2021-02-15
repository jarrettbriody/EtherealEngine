#pragma once
#include "pch.h"
#include "Vertex.h"
#include "Lights.h"
#include "SimpleShader.h"
#include "Entity.h"
#include "Camera.h"
#include "DebugLines.h"

using namespace std;

#define MAX_LIGHTS 32

struct RenderObject{
	Entity* entity;
	Mesh* mesh;
	Material* material;
};

class Renderer
{
private:
	static Renderer* instance;

	vector<Entity*>* entities = nullptr;
	RenderObject* renderObjects;
	int renderObjectCount = 0;
	int maxRenderObjects = 0;

	Camera* camera = nullptr;
	map<string, Camera*> cameras;
	unsigned int cameraCount = 0;

	map<string, Light*> lights;
	//map<string, Shadow> shadows;
	unsigned int lightCount = 0;
	bool shadowsEnabled = true;

	unsigned int shadowMapResolution = 2048;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11SamplerState* shadowSampler;
	ID3D11RasterizerState* shadowRasterizer;
	SimpleVertexShader* shadowVS = nullptr;
	SimpleVertexShader* debugLineVS = nullptr;
	SimplePixelShader* debugLinePS = nullptr;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;

	Renderer();
	~Renderer();
public:
	static bool SetupInstance();
	static Renderer* GetInstance();
	static bool DestroyInstance();

	void SetEntities(vector<Entity*>* entities);
	void SetShadowVertexShader(SimpleVertexShader* shadowVS);
	void SetDebugLineVertexShader(SimpleVertexShader* debugLineVS);
	void SetDebugLinePixelShader(SimplePixelShader* debugLinePS);

	void InitShadows();
	void ToggleShadows(bool toggle);
	void SetShadowMapResolution(unsigned int res);

	void ClearFrame();
	void RenderFrame();
	void PresentFrame();
	void RenderDebugLines();
	void RenderShadowMap();

	bool AddCamera(string name, Camera* newCamera);
	bool RemoveCamera(string name);
	Camera* GetCamera(string name);
	bool EnableCamera(string name);

	bool AddLight(std::string name, Light* newLight);
	bool RemoveLight(std::string name);
	void SendAllLightsToShader(SimplePixelShader* pixelShader);
	Light* GetLight(string name);

	void AddRenderObject(Entity* e, Mesh* mesh, Material* mat = nullptr);
};
