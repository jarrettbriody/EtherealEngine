#pragma once
#include "ScriptManager.h"
#include "GameManager.h"
#include "FPSController.h"

class BloodIcicle : public ScriptManager
{
	map<string, Entity*>* eMap;

	Entity* closestChild;

	GameManager* gameManagerScript;
	FPSController* fpsControllerScript;

	bool bodyPartPinned = false;

	XMFLOAT4X4 translation;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);
};

