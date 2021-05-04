#pragma once
#include "pch.h"
#include "SimpleShader.h"
#include "DebugLines.h"
#include "Utility.h"
#include "Mesh.h"
#include "Material.h"
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

struct ScriptPair {
	Entity* e;
	string script;
};

struct SceneLoaderCallback {
	virtual void callback(Entity* e) {};
};

class SceneLoader
{
private:
	static SceneLoader* instance;

	map<string, BulletColliderShape> bulletColliders =
	{
	   { "BOX", BulletColliderShape::BOX },
	   { "CAPSULE", BulletColliderShape::CAPSULE},
	};

	map<string, int> sceneLineTypes = {
		{ "ENTITY", 0 },
		{ "MESH", 1 },
		{ "MATERIAL", 2 },
		{ "TEXTURE", 3 },
		{ "VSHADER", 4 },
		{ "PSHADER", 5 },
		{ "CSHADER", 6 },
		{ "SKYBOX", 7 },
		{ "DIRLIGHT", 8 },
		{ "CPUPARTICLE", 9 },
		{ "GPUPARTICLE", 10 },
	};

	map<string, ShaderType> shaderTypes = {
		{ "DEFAULT", ShaderType::DEFAULT },
		{ "NORMAL", ShaderType::NORMAL },
		{ "MODIFY_VERTS", ShaderType::MODIFY_VERTS },
	};

	string modelPath = "../../Assets/Models/";

	//Scene loading regular expressions
	regex commentedLineRegex = regex("//.*"); //checking if line is commented

	regex typeRegex = regex("TYPE=\"(\\w+)\""); //getting line type

	regex skyboxRegex = regex("dds=\"(\\w+)\""); //get dds name

	regex lightPosRegex = regex("P\\(.*\\)");
	regex lightDirRegex = regex("D\\(.*\\)");
	regex lightColorRegex = regex("C\\(.*\\)");
	regex lightIntensityRegex = regex("intensity=\"(\\d*\\.\\d*|\\d+)\"");

	regex shaderFileRegex = regex("cso=\"(\\w+)\""); //get cso file name
	regex shaderTypeRegex = regex("shaderType=\"(\\w+)\""); //get shader type

	regex pathRegex = regex("path=\"(.+)\""); //get path to thing

	regex texArrayRegex = regex("array=\"(\\w+)\"");

	regex vShaderRegex = regex("vShader=\"(\\w+)\"");
	regex pShaderRegex = regex("pShader=\"(\\w+)\"");
	regex ambientTexRegex = regex("ambientTexture=\"(\\w+)\"");
	regex diffuseTexRegex = regex("diffuseTexture=\"(\\w+)\"");
	regex specColorTexRegex = regex("specularColorTexture=\"(\\w+)\"");
	regex specHighlightTexRegex = regex("specularHighlightTexture=\"(\\w+)\"");
	regex alphaTexRegex = regex("alphaTexture=\"(\\w+)\"");
	regex normalTexRegex = regex("normalTexture=\"(\\w+)\"");
	regex ambientColorRegex = regex("ambientColor\\(.*\\)");
	regex diffuseColorRegex = regex("diffuseColor\\(.*\\)");
	regex specularColorRegex = regex("specularColor\\(.*\\)");
	regex specularExponentRegex = regex("specularExponent=\"(\\d*\\.\\d*|\\d+)\"");
	regex transparencyRegex = regex("transparency=\"(\\d*\\.\\d*|\\d+)\"");
	regex illuminationRegex = regex("illumination=\"(\\d+)\"");
	regex ssaoRegex = regex("ssao=\"(\\w+)\"");
	regex hbaoPlusRegex = regex("hbaoPlus=\"(\\w+)\"");

	regex entityNameRegex = regex("name=\"([\\w|\\s]+)\""); //getting entity name
	regex objNameRegex = regex("obj=\"(\\w+)\""); //getting obj model name
	regex materialNameRegex = regex("material=\"(\\w+)\"");
	regex repeatTextureRegex = regex("repeatTexture=\"(\\d*\\.\\d*|\\d+),(\\d*\\.\\d*|\\d+)\"");
	regex uvOffsetRegex = regex("uvOffset=\"(-?\\d*\\.\\d*|-?\\d+),(-?\\d*\\.\\d*|-?\\d+)\"");
	regex tagNameRegex = regex("tags=\"([\\w+|,]+)\""); //for getting the entity tags
	regex layerNameRegex = regex("layers=\"([\\w+|,]+)\""); //for getting the entity layer
	regex scriptNamesRegex = regex("scripts=\"([\\w+|,]+)\""); //getting script names associated with entity
	regex scriptNamesIteratorRegex = regex("\\w+"); //iterating over each script name associated with entity
	regex collidersEnabledRegex = regex("colliders=\"(\\w+)\"");
	regex colliderTypeRegex = regex("colliderType=\"(\\w+)\"");
	regex massRegex = regex("mass=\"(\\d*\\.\\d*|\\d+)\"");
	regex debugRegex = regex("debug=\"(\\w+)\"");
	regex raaRegex = regex("rotateAroundAxis=\"(\\w+)\"");
	regex clampTexRegex = regex("clampTex=\"(\\w+)\"");
	regex transformationDataRegex = regex("P\\(.*\\)R\\(.*\\)S\\(.*\\)");
	regex quaternionRegex = regex("Q\\(.*\\)");
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

	SceneLoader();
	~SceneLoader();

	XMFLOAT3 Float3FromString(string str);

	template <class SHADER>
	SHADER* LoadShader(string shaderName, string shaderFileName, map<string,SHADER*>& shaderMap, ShaderType shaderType = ShaderType::DEFAULT);

	Mesh* LoadMesh(string meshName, string meshPath, map<string,Mesh*>& meshMap);

	ID3D11ShaderResourceView* LoadTexture(string texName, string texPath, map<string, ID3D11ShaderResourceView*>& texMap, bool keepTex2D = false);

	Material* CreateMaterial(string name, MaterialData matData, string vertShaderName, string pixelShaderName, map<string, Material*>& matMap);
public:
	MemoryAllocator* EEMemoryAllocator = nullptr;
	Renderer* EERenderer = nullptr;
	LightHandler* EELightHandler = nullptr;

	vector<ScriptPair> scriptPairs;

	map<string, SimpleVertexShader*> vertexShadersMap;
	map<string, SimplePixelShader*> pixelShadersMap;
	map<string, SimpleComputeShader*> computeShadersMap;

	map<string, bool> utilizedMeshesMap;
	map<string, bool> utilizedMaterialsMap;
	map<string, bool> utilizedTexturesMap;

	map<string, vector<string>> materialTextureAssociationMap;

	map<string, Mesh*> defaultMeshesMap;
	map<string, Mesh*> generatedMeshesMap;

	map<string, ID3D11ShaderResourceView*> defaultTexturesMap;
	map<string, ID3D11ShaderResourceView*> generatedTexturesMap;
	map<string, ID3D11Texture2D*> texture2DMap;

	map<string, Material*> defaultMaterialsMap;
	map<string, Material*> generatedMaterialsMap;

	map<string, Entity*> sceneEntitiesMap;
	map<string, vector<Entity*>> sceneEntitiesTagMap;
	map<string, vector<Entity*>> sceneEntitiesLayerMap;
	vector<Entity*> sceneEntities;

	SceneLoaderCallback* garbageCollectCallback;

	static bool SetupInstance();
	static SceneLoader* GetInstance();
	static bool DestroyInstance();

	void GarbageCollect();

	void LoadAssetPreloadFile();

	MESH_TYPE AutoLoadOBJMTL(string name);
	void LoadScene(string sceneName = "scene");
	void SetModelPath(string path);
	void SetScriptLoader(void (*callback)(Entity* e, string script));

	Entity* CreateEntity(EntityCreationParameters& para);
	std::vector<Entity*> SplitMeshIntoChildEntities(Entity* e, float componentMass);

	void DestroyEntity(string entityName);
	void DestroyEntitiesByTag(string tag);
};
