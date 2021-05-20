#pragma once
#include "ScriptManager.h"
#include "AmbientParticles.h"

class BloodPool : public ScriptManager
{
	PhysicsWrapper entityWrapper;

	btGhostObject* poolGhostObject;
	
	float growthScalar = 5.0f;
	XMFLOAT3 finalScale = XMFLOAT3(2.5f, 4.0f, 2.5f);
	float timeout = 15.0f;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);

	~BloodPool();
};

