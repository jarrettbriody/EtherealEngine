#include "ScriptManager.h"

vector<ScriptManager*> ScriptManager::scriptFunctions;
map<string, vector<ScriptManager*>> ScriptManager::scriptFunctionsMap;
map<string, Entity*>* ScriptManager::sceneEntitiesMap;

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
