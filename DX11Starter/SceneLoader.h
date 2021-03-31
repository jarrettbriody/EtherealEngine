#pragma once
#include "pch.h"
#include "SimpleShader.h"
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
	string tagName = "";
	string layerName = "";
	string meshName = "";
	string materialName = "";
	string scriptNames[8];
	unsigned int scriptCount = 0;
	XMFLOAT3 position = ZERO_VECTOR3;
	XMFLOAT3 rotationRadians = ZERO_VECTOR3;
	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	float entityMass = 0.0f;
	bool initRigidBody = true;
	BulletColliderShape bulletColliderShape = BulletColliderShape::BOX;
	bool collisionsEnabled = true;
	bool drawEntity = true;
	bool drawShadow = true;
};

static map<string, BulletColliderShape> bulletColliders = { { "BOX", BulletColliderShape::BOX }, {"CAPSULE", BulletColliderShape::CAPSULE} };

class SceneLoader
{
private:
	static SceneLoader* instance;

	SceneLoader();
	~SceneLoader();


	//Scene loading regular expressions
	regex commentedLineRegex = regex("//.*"); //checking if line is commented
	regex entityNameRegex = regex("name=\"([\\w|\\s]+)\""); //getting entity name
	regex objNameRegex = regex("obj=\"(\\w+)\""); //getting obj model name
	regex materialNameRegex = regex("material=\"(\\w+)\"");
	regex repeatTextureRegex = regex("repeatTexture=\"(\\d*\\.\\d*|\\d+),(\\d*\\.\\d*|\\d+)\"");
	regex tagNameRegex = regex("tag=\"(\\w+)\""); //for getting the entity tag
	regex layerNameRegex = regex("layer=\"(\\w+)\""); //for getting the entity layer
	regex scriptNamesRegex = regex("scripts=\"([\\w+|,]+)\""); //getting script names associated with entity
	regex scriptNamesIteratorRegex = regex("\\w+"); //iterating over each script name associated with entity
	regex collidersEnabledRegex = regex("colliders=\"(\\w+)\"");
	regex colliderTypeRegex = regex("colliderType=\"(\\w+)\"");
	regex massRegex = regex("mass=\"(\\d*\\.\\d*|\\d+)\"");
	regex debugRegex = regex("debug=\"(\\w+)\"");
	regex transformationDataRegex = regex("P\\(.*\\)R\\(.*\\)S\\(.*\\)");
	regex transformNumIteratorRegex = regex("-\\d*\\.\\d*|\\d*\\.\\d*|-\\d+|\\d+"); //for iterating over each line to get the float values for transformations

	//Material loading regular expressions
	regex newMtlRgx = regex("^(newmtl\\s+)");
	regex ambientColorRgx = regex("^(Ka\\s+)");
	regex diffuseColorRgx = regex("^(Kd\\s+)");
	regex specularColorRgx = regex("^(Ks\\s+)");
	regex specularExpRgx = regex("^(Ns\\s+)");
	regex dTransparencyRgx = regex("^(d\\s+)");
	regex trTransparencyRgx = regex("^(Tr\\s+)");
	regex illuminationRgx = regex("^(illum\\s+)");
	regex ambientTextureRgx = regex("^(map_Ka\\s+)");
	regex diffuseTextureRgx = regex("^(map_Kd\\s+)");
	regex specularColorTextureRgx = regex("^(map_Ks\\s+)");
	regex specularHighlightTextureRgx = regex("^(map_Ns\\s+)");
	regex alphaTextureRgx = regex("^(map_d\\s+)");
	regex normalTextureRgx = regex("^(map_Bump\\s+)");

	void (*scriptCallback)(Entity* e, string script);
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

	string modelPath = "../../Assets/Models/";

	static bool SetupInstance();
	static SceneLoader* GetInstance();
	static bool DestroyInstance();

	void LoadShaders();
	void LoadDefaultMeshes();
	void LoadDefaultTextures();
	void LoadDefaultMaterials();

	MESH_TYPE AutoLoadOBJMTL(string name);
	void LoadScene(string sceneName = "scene");
	void SetModelPath(string path);
	void SetScriptLoader(void (*callback)(Entity* e, string script));

	Entity* CreateEntity(EntityCreationParameters& para);
	void SplitMeshIntoChildEntities(Entity* e, float componentMass);
};

