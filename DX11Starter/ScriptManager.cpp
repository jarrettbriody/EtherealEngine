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
