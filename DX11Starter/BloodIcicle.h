#pragma once
#include "ScriptManager.h"
#include "GameManager.h"

class BloodIcicle : public ScriptManager
{
	map<string, Entity*>* eMap;

	Entity* closestChild;

	Entity* gameManager;

	bool bodyPartPinned = false;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);
};

