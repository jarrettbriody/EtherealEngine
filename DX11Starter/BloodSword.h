#pragma once
#include "ScriptManager.h"

enum class SwordState
{
	SetTrajectory, Slashing, Reset, Idle
};

class BloodSword : public ScriptManager
{
	map<string, Entity*>* eMap;

	SwordState ss;
	
	btVector3 defaultPos;
	btVector3 startPos;
	btVector3 endPos;

	void Init();

	void Update();

	void StartSwing();

	void SetLerpPositions();

	void Slash();

	void ResetSword();

	void OnCollision(btCollisionObject* other);
};

