#pragma once
#include "ScriptManager.h"

class BloodIcicle : public ScriptManager
{
	map<string, Entity*>* eMap;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);
};

