#pragma once
#include "ScriptManager.h"

using namespace std;

class TestScript : public ScriptManager
{
	map<string, Entity*>* eMap = ScriptManager::sceneEntitiesMap;

	string test = "Scope";

	float totalTime = 0.0f;
	int waveCounter = 0;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);
};

