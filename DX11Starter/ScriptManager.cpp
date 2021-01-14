#include "ScriptManager.h"

vector<ScriptManager*> ScriptManager::scriptFunctions;
map<string, vector<ScriptManager*>> ScriptManager::scriptFunctionsMap;
map<string, Entity*>* ScriptManager::sceneEntitiesMap; 
vector<Entity*>* ScriptManager::sceneEntities;
Renderer* ScriptManager::renderer;
SceneLoader* ScriptManager::sceneLoader;

void ScriptManager::CallInit()
{
	if (initialized)
		Init();
}

void ScriptManager::CallUpdate()
{
	if (initialized)
		Update();
}

void ScriptManager::CallOnMouseDown(WPARAM buttonState, int x, int y)
{
	if (initialized)
		OnMouseDown(buttonState, x, y);
}

void ScriptManager::CallOnMouseUp(WPARAM buttonState, int x, int y)
{
	if (initialized)
		OnMouseUp(buttonState, x, y);
}

void ScriptManager::CallOnMouseMove(WPARAM buttonState, int x, int y)
{
	if (initialized)
		OnMouseMove(buttonState, x, y);
}

void ScriptManager::CallOnMouseWheel(float wheelDelta, int x, int y)
{
	if (initialized)
		OnMouseWheel(wheelDelta, x, y);
}

void ScriptManager::Setup(string name, Entity* e)
{
	this->name = name;
	entity = e;

	scriptFunctions.push_back(this);
	if (!ScriptManager::scriptFunctionsMap.count(name)) {
		ScriptManager::scriptFunctionsMap.insert({ this->name, vector<ScriptManager*>() });
	}
	ScriptManager::scriptFunctionsMap[this->name].push_back(this);

	initialized = true;
}

ScriptManager::ScriptManager()
{
}

ScriptManager::~ScriptManager()
{
}

void ScriptManager::CreateEntity(EntityCreationParameters& para)
{
	Entity* e;
	Mesh* mesh;
	Material* mat;
	if (para.entityName == "") return;
	if (para.meshName != "") {
		if (sceneLoader->generatedMeshesMap.count(para.meshName)) {
			mesh = sceneLoader->generatedMeshesMap[para.meshName];
			e = new Entity(para.entityName, mesh);
		}
		else if (sceneLoader->defaultMeshesMap.count(para.meshName)) {
			mesh = sceneLoader->defaultMeshesMap[para.meshName];
			e = new Entity(para.entityName, mesh);
		}
		else return;

		if (para.materialName != "") {
			if (sceneLoader->generatedMaterialsMap.count(para.materialName)) {
				mat = sceneLoader->generatedMaterialsMap[para.materialName];
				e->AddMaterial(mat);
			}
			else if (sceneLoader->defaultMaterialsMap.count(para.materialName)) {
				mat = sceneLoader->defaultMaterialsMap[para.materialName];
				e->AddMaterial(mat);
			}
			else {
				para.materialName = "DEFAULT";
				mat = sceneLoader->defaultMaterialsMap["DEFAULT"];
				e->AddMaterial(mat);
			}

			e->AddMaterialNameToMesh(para.materialName);
		}
	}
	else {
		e = new Entity(para.entityName);
	}
	e->SetPosition(para.position);
	e->SetRotation(para.rotationRadians);
	e->SetScale(para.scale);
	if(para.initRigidBody)
		e->InitRigidBody(sceneLoader->dynamicsWorld);
	sceneLoader->sceneEntitiesMap.insert({ para.entityName, e });
	sceneLoader->sceneEntities.push_back(e);
	if (para.drawEntity)
		renderer->AddRenderObject(e, mesh, mat);
	if (para.drawShadow)
		e->ToggleShadows(true);
}
