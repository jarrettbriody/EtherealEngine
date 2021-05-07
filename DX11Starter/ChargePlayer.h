#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"

class ChargePlayer : public Behavior
{
private:
	Entity* player;
	Entity* enemy;
	float chargeSpeed;

public:
	ChargePlayer(Entity* e, Entity* p, float cSpeed) : enemy(e), player(p), chargeSpeed(cSpeed) {}
	~ChargePlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

