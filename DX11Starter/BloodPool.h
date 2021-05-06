#pragma once
#include "ScriptManager.h"

class BloodPool : public ScriptManager
{
	PhysicsWrapper entityWrapper;

	btGhostObject* poolGhostObject;
	
	float growthScalar = 5.0f;
	XMFLOAT3 finalScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);

	~BloodPool();
};

