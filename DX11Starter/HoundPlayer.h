#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"

class HoundPlayer : public Behavior
{
private:
	Entity* enemy;
	float pounceSpeed;

public:
	HoundPlayer(Entity* e, float pSpeed) : enemy(e), pounceSpeed(pSpeed) {}
	~HoundPlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

