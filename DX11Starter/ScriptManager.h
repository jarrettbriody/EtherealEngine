#pragma once
#include "Entity.h"
class ScriptManager
{
public:
	static vector<ScriptManager*> scriptFunctions;
	static map<string, vector<ScriptManager*>> scriptFunctionsMap;
	static map<string, Entity*>* sceneEntitiesMap;

	bool destroyed = false;

	void CallInit();
	void CallUpdate();

	void Setup(string name, Entity* e);

	virtual ~ScriptManager();
protected:
	bool initialized = false;
	string name;
	Entity* entity;
	ScriptManager();
	virtual void Init() = 0;
	virtual void Update() = 0;
};

