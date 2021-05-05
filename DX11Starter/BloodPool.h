#pragma once
#include "ScriptManager.h"

class BloodPool : public ScriptManager
{
	PhysicsWrapper entityWrapper;

	btGhostObject* poolGhostObject;
	
	float growthScalar = 5.0f;
	XMFLOAT3 finalScale = XMFLOAT3(3.5f, 0.1f, 3.5f);

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);

	~BloodPool();
};

