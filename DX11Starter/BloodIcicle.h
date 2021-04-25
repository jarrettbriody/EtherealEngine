#pragma once
#include "ScriptManager.h"

class BloodIcicle : public ScriptManager
{
	map<string, Entity*>* eMap;

	btVector3 initialImpulse;

	Entity* closestChild;

	bool bodyPartPinned = false;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);
};

