#pragma once
#include "Renderer.h"
#include "SceneLoader.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Grid.h"

class ScriptManager
{
public:
	static vector<ScriptManager*> scriptFunctions;
	static map<string, map<string,ScriptManager*>> scriptFunctionsMap;
	static map<string, vector<ScriptManager*>> scriptFunctionsMapVector;
	static map<string, Entity*>* sceneEntitiesMap;
	static vector<Entity*>* sceneEntities;
	static Renderer* EERenderer;
	static SceneLoader* EESceneLoader;
	static float deltaTime;

	bool destroyed = false;

	bool inputEnabled = false;

	bool GetIsInitialized() { return initialized; }

	void CallInit();
	void CallUpdate(float deltaTime);
	void CallOnCollision(btCollisionObject* other);

	void Setup(Entity* e, string scriptName);

	virtual ~ScriptManager();
protected:
	bool initialized = false;
	bool setup = false;
	string name;
	Entity* entity;
	ScriptManager();
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void OnCollision(btCollisionObject* other) {};
	Entity* CreateEntity(EntityCreationParameters& para);
};

