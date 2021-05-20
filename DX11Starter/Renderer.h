#pragma once
#include "pch.h"
#include "Vertex.h"
#include "Lights.h"
#include "SimpleShader.h"
#include "Entity.h"
#include "Camera.h"
#include "DebugLines.h"
#include "DecalHandler.h"
#include "RendererStructs.h"
#include "LightHandler.h"

using namespace std;

class Renderer
{
private:
	static Renderer* instance;

	LightHandler* EELightHandler = nullptr;

	vector<Entity*>* entities = nullptr;
	RenderObject* renderObjects;
	RenderObject* transparentObjects;
	int renderObjectCount = 0;
	int transparentObjectCount = 0;
	int maxRenderObjects = 0;
	int maxTransparentObjects = 0;

	map<SimplePixelShader*, bool> lightsSentToShader;

	RendererShaders shaders = {};

	Mesh* cube = nullptr;
	Mesh* invCube = nullptr;
	Mesh* sphere = nullptr;
	Mesh* cone = nullptr;

	Camera* camera = nullptr;
	map<string, Camera*> cameras;
	unsigned int cameraCount = 0;

	//map<string, Light*> lights;
	//map<string, Shadow> shadows;
	//unsigned int lightCount = 0;

	ID3D11ShaderResourceView* decals[8];

	Renderer();
	~Renderer();

	void CalcShadowMatrices(unsigned int cascadeIndex = 0);
public:
	static bool SetupInstance();
	static Renderer* GetInstance();
	static bool DestroyInstance();

	void GarbageCollect();

	ID3D11BlendState* blendState = nullptr;
	ID3D11DepthStencilState* alwaysDrawDepthStencil = nullptr;
	ShadowComponents shadowComponents = {};
	DepthStencilComponents depthStencilComponents = {};
	HBAOPlusComponents hbaoPlusComponents = {};
	SkyboxComponents skyboxComponents = {};
	PostProcessComponents postProcessComponents = {};
	UIComponents uiComponents = {};

	void SetEntities(vector<Entity*>* entities);
	void SetRendererShaders(RendererShaders rShaders);
	void SetDecals(ID3D11ShaderResourceView* decals[8]);
	void SetMeshes(Mesh* cube, Mesh* invCube, Mesh* sphere, Mesh* cone);

	void InitDepthStencil();
	void InitHBAOPlus();
	void InitShadows(unsigned int cascadeCount = 3);
	void InitSkybox();
	void InitPostProcessRTV();
	void SetSkybox(ID3D11ShaderResourceView* srv);
	void SetShadowCascadeInfo(unsigned int cascadeIndex, unsigned int resolution, float nearPlane, float farPlane, float width, float height);

	void InitBlendState();
	void ToggleBlendState(bool toggle);

	void ClearFrame();
	void RenderFrame();
	void RenderDecals();
	void RenderHBAOPlus();
	void PresentFrame();
	void RenderDebugLines();
	void RenderShadowMap();
	void RenderDepthStencil();
	void RenderSkybox();
	void RenderTransparents();
	void RenderPostProcess();
	void RenderUI();

	bool AddCamera(string name, Camera* newCamera);
	bool RemoveCamera(string name);
	Camera* GetCamera(string name);
	bool EnableCamera(string name);

	void SendSSAOKernelToShader(SimplePixelShader* pixelShader);

	void AddRenderObject(Entity* e, Mesh* mesh, Material* mat = nullptr);
	void AddTransparentObject(Entity* e, Mesh* mesh, Material* mat = nullptr);
	void ClearRenderer();

	void SetRenderObjectCallback(Entity* e, RendererCallback* callback);
	void SetPostProcess(bool toggle, RendererCallback* callback = nullptr, unsigned int index = 0);
	void SetRenderUICallback(bool toggle, Utility::Callback* cb, unsigned int index = 0);
};
