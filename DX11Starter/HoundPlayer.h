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
	bool* hooked;

public:
	HoundPlayer(Entity* e, Entity* p, float pSpeed, float* cdTime, float maxCdTime, bool* hooked) : enemy(e), player(p), pounceSpeed(pSpeed), cooldownTimer(cdTime), maxCooldownTime(maxCdTime), hooked(hooked) {}
	~HoundPlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

