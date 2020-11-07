#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Terrain.h"
#include "TerrainMaterial.h"
#include "Water.h"
#include "WaterMaterial.h"
#include "Entity.h"
#include <map>
#include <vector>

class SceneLoader
{
private:
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11SamplerState* sampler;
public:
	map<string, SimpleVertexShader*> vertexShadersMap;
	map<string, SimplePixelShader*> pixelShadersMap;

	map<string, bool> utilizedMeshesMap;
	map<string, bool> utilizedMaterialsMap;
	map<string, bool> utilizedTexturesMap;

	map<string, Mesh*> defaultMeshesMap;
	map<string, Mesh*> generatedMeshesMap;

	map<string, ID3D11ShaderResourceView*> defaultTexturesMap;
	map<string, ID3D11ShaderResourceView*> generatedTexturesMap;

	map<string, Material*> defaultMaterialsMap;
	map<string, Material*> generatedMaterialsMap;

	map<string, Entity*> sceneEntitiesMap;
	vector<Entity*> sceneEntities;

	SceneLoader(ID3D11Device* device, ID3D11DeviceContext*	context, ID3D11SamplerState* sampler);
	~SceneLoader();

	void LoadShaders();
	void LoadDefaultMeshes();
	void LoadDefaultTextures();
	void LoadDefaultMaterials();

	void BuildDefaultEntity(string entityName, string objName, Entity* e);

	Utility::MESH_TYPE AutoLoadOBJMTL(string name);
	void LoadScene(string sceneName = "scene");
};
