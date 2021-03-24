#include "pch.h"
#include "ScriptManager.h"

vector<ScriptManager*> ScriptManager::scriptFunctions;
map<string, vector<ScriptManager*>> ScriptManager::scriptFunctionsMap;
map<string, Entity*>* ScriptManager::sceneEntitiesMap; 
vector<Entity*>* ScriptManager::sceneEntities;
Renderer* ScriptManager::EERenderer;
SceneLoader* ScriptManager::EESceneLoader;
float ScriptManager::deltaTime;

void ScriptManager::CallInit()
{
	if (initialized)
		Init();
}

void ScriptManager::CallUpdate(float deltaTime)
{
	this->deltaTime = deltaTime;
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

void ScriptManager::CallOnCollision(btCollisionObject* other)
{
	if (initialized)
		OnCollision(other);
}

void ScriptManager::Setup(Entity* e)
{
	entity = e;
	this->name = e->GetName();

	this->EERenderer = Renderer::GetInstance();
	this->EESceneLoader = SceneLoader::GetInstance();
	this->sceneEntities = &this->EESceneLoader->sceneEntities;
	this->sceneEntitiesMap = &this->EESceneLoader->sceneEntitiesMap;

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
