#include "pch.h"
#include "ScriptManager.h"

vector<ScriptManager*> ScriptManager::scriptFunctions;
map<string, map<string, ScriptManager*>> ScriptManager::scriptFunctionsMap;
map<string, vector<ScriptManager*>> ScriptManager::scriptFunctionsMapVector;
map<string, Entity*>* ScriptManager::sceneEntitiesMap; 
vector<Entity*>* ScriptManager::sceneEntities;
Renderer* ScriptManager::EERenderer;
SceneLoader* ScriptManager::EESceneLoader;
double ScriptManager::deltaTime = 0.0;

void ScriptManager::CallInit()
{
	if (setup) {
		Init();
		initialized = true;
	}
}

void ScriptManager::CallUpdate()
{
	if (setup)
		Update();
}

void ScriptManager::CallOnCollision(btCollisionObject* other)
{
	if (setup)
		OnCollision(other);
}

void ScriptManager::Setup(Entity* e, string scriptName)
{
	entity = e;
	this->name = scriptName;
	string entityName = e->GetName();

	this->EERenderer = Renderer::GetInstance();
	this->EESceneLoader = SceneLoader::GetInstance();
	this->sceneEntities = &this->EESceneLoader->SceneEntities;
	this->sceneEntitiesMap = &this->EESceneLoader->SceneEntitiesMap;

	//add this script (and thereby all script function pointers) to the list of scripts
	scriptFunctions.push_back(this);
	if (!ScriptManager::scriptFunctionsMap.count(entityName)) {
		ScriptManager::scriptFunctionsMap.insert({ entityName, map<string,ScriptManager*>() });
		ScriptManager::scriptFunctionsMapVector.insert({ entityName, vector<ScriptManager*>() });
	}
	ScriptManager::scriptFunctionsMap[entityName].insert({scriptName, this});
	ScriptManager::scriptFunctionsMapVector[entityName].push_back(this);

	setup = true;
}

ScriptManager::ScriptManager()
{
}

ScriptManager::~ScriptManager()
{
}

Entity* ScriptManager::CreateEntity(EntityCreationParameters& para)
{
	return EESceneLoader->CreateEntity(para);
}
