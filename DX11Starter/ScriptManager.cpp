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

void ScriptManager::GarbageCollect()
{
	unsigned int start = ScriptManager::scriptFunctions.size();
	for (unsigned int i = start; i > 0; i--)
	{
		ScriptManager* s = scriptFunctions[i - 1];
		if (s->destroyed) {
			scriptFunctions.erase(scriptFunctions.begin() + i - 1);
			delete s;
		}
	}
}

void ScriptManager::DestroyScriptsByOwner(string entityName)
{
	vector<ScriptManager*> scriptFuncs = scriptFunctionsMapVector[entityName];
	size_t cnt = scriptFuncs.size();
	for (size_t j = cnt; j > 0; j--)
	{
		scriptFuncs[j - 1]->destroyed = true;
	}
	scriptFunctionsMap.erase(entityName);
	scriptFunctionsMapVector.erase(entityName);
}

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
	this->sceneEntities = &this->EESceneLoader->sceneEntities;
	this->sceneEntitiesMap = &this->EESceneLoader->sceneEntitiesMap;

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
