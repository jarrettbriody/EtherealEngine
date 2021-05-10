#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"

class PlayerIsInRange : public Behavior
{
private:
	Entity* enemy;
	Entity* player;
	float minimumDistance;
	bool* inRange;
public:
	PlayerIsInRange(Entity* e, Entity* p, float distance, bool* isPlayerInRange) : enemy(e), player(p), minimumDistance(distance), inRange(isPlayerInRange) {}
	~PlayerIsInRange() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

