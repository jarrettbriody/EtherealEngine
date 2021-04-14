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

using namespace std;

class Renderer
{
private:
	static Renderer* instance;

	vector<Entity*>* entities = nullptr;
	RenderObject* renderObjects;
	RenderObject* transparentObjects;
	int renderObjectCount = 0;
	int transparentObjectCount = 0;
	int maxRenderObjects = 0;
	int maxTransparentObjects = 0;
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
	ID3D11BlendState* blendState;
	ShadowComponents shadowComponents;
	DepthStencilComponents depthStencilComponents;
	HBAOPlusComponents hbaoPlusComponents;
	SkyboxComponents skyboxComponents;

	ID3D11ShaderResourceView* decals[8];

	Renderer();
	~Renderer();

	void CalcShadowMatrices(unsigned int cascadeIndex = 0);
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
	void InitShadows(unsigned int cascadeCount = 3);
	void InitSkybox();
	void SetSkybox(ID3D11ShaderResourceView* srv);
	void SetShadowCascadeInfo(unsigned int cascadeIndex, unsigned int resolution, float nearPlane, float farPlane, float width, float height, float maxRange);

	void InitBlendState();
	void ToggleBlendState(bool toggle);

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
	void AddTransparentObject(Entity* e, Mesh* mesh, Material* mat = nullptr);

	void SetRenderObjectCallback(Entity* e, RendererCallback* callback);
};
