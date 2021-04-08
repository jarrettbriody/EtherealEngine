#pragma once
#include "ScriptManager.h"

class BloodIcicle : public ScriptManager
{
	map<string, Entity*>* eMap;

	btVector3 initialImpulse;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);
};

