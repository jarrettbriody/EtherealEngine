#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"

class Idle : public Behavior
{
private:
	Entity* enemy;
	bool* inCombat;
	float currentSpeed;
	float dampingScalar = 0.9f;
	XMFLOAT3 directionVector;
	btVector3 movementDirection;
public:
	Idle(Entity* e, bool* combat) : enemy(e), inCombat(combat) {}
	~Idle() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};