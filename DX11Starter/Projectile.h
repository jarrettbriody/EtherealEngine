#pragma once
#include "ScriptManager.h"

class Projectile : public ScriptManager
{
	map<string, Entity*>* eMap;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);
};

