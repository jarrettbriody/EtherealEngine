#pragma once
#include "pch.h"
#include "DebugLines.h"
#include "Utility.h"
#include "Mesh.h"
#include "Material.h"
#include "Terrain.h"
#include "TerrainMaterial.h"
#include "Water.h"
#include "WaterMaterial.h"
#include "Entity.h"
#include "Config.h"
#include "MemoryAllocator.h"
#include "Renderer.h"

using namespace Utility;

struct EntityCreationParameters {
	string entityName = "";
	string meshName = "";
	string materialName = "";
	XMFLOAT3 position = ZERO_VECTOR3;
	XMFLOAT3 rotationRadians = ZERO_VECTOR3;
	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	bool initRigidBody = true;
	bool drawEntity = true;
	bool drawShadow = true;
};

class SceneLoader
{
private:
	static SceneLoader* instance;

	SceneLoader();
	~SceneLoader();
public:
	MemoryAllocator* EEMemoryAllocator = nullptr;
	Renderer* EERenderer = nullptr;

	map<string, SimpleVertexShader*> vertexShadersMap;
	map<string, SimplePixelShader*> pixelShadersMap;

	map<string, bool> utilizedMeshesMap;
	map<string, bool> utilizedMaterialsMap;
	map<string, bool> utilizedTexturesMap;

	map<string, vector<string>> materialTextureAssociationMap;

	map<string, Mesh*> defaultMeshesMap;
	map<string, Mesh*> generatedMeshesMap;

	map<string, ID3D11ShaderResourceView*> defaultTexturesMap;
	map<string, ID3D11ShaderResourceView*> generatedTexturesMap;

	map<string, Material*> defaultMaterialsMap;
	map<string, Material*> generatedMaterialsMap;

	map<string, Entity*> sceneEntitiesMap;
	vector<Entity*> sceneEntities;

	static bool SetupInstance();
	static SceneLoader* GetInstance();
	static bool DestroyInstance();

	void LoadShaders();
	void LoadDefaultMeshes();
	void LoadDefaultTextures();
	void LoadDefaultMaterials();

	void BuildDefaultEntity(string entityName, string objName, Entity* e);

	Utility::MESH_TYPE AutoLoadOBJMTL(string name);
	void LoadScene(string sceneName = "scene");

	Entity* CreateEntity(EntityCreationParameters& para);
};

