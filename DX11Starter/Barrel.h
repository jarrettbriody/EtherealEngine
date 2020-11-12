#pragma once
#include "ScriptManager.h"
#include <iostream>

using namespace std;

class TestScript : public ScriptManager
{
	map<string, Entity*>* eMap = ScriptManager::sceneEntitiesMap;

	string test = "Scope";

	void Init();

	void Update();
};

