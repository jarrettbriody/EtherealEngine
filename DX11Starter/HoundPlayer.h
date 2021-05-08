#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"

class HoundPlayer : public Behavior
{
private:
	Entity* enemy;
	Entity* player;
	float pounceSpeed;
	float* cooldownTimer;
	float maxCooldownTime;

public:
	HoundPlayer(Entity* e, Entity* p, float pSpeed, float* cdTime, float maxCdTime) : enemy(e), player(p), pounceSpeed(pSpeed), cooldownTimer(cdTime), maxCooldownTime(maxCdTime) {}
	~HoundPlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

