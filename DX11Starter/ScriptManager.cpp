#include "ScriptManager.h"

vector<ScriptManager*> ScriptManager::scriptFunctions;
map<string, vector<ScriptManager*>> ScriptManager::scriptFunctionsMap;
map<string, Entity*>* ScriptManager::sceneEntitiesMap; 
vector<Entity*>* ScriptManager::sceneEntities;
Renderer* ScriptManager::EERenderer;
SceneLoader* ScriptManager::EESceneLoader;

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

void ScriptManager::Setup(Entity* e)
{
	entity = e;
	this->name = e->GetName();

	//add this script (and thereby all script function pointers) to the list of scripts
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
	EESceneLoader->CreateEntity(para);
}
