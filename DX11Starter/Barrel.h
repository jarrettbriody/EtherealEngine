#pragma once
#include "ScriptManager.h"

using namespace std;

class TestScript : public ScriptManager
{
	map<string, Entity*>* eMap = ScriptManager::sceneEntitiesMap;

	string test = "Scope";

	void Init();

	void Update();
};

