#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"

class FacePlayer : public Behavior
{
private:
	Entity* enemy;
	Entity* player;
	float turnSpeed;
public:
	FacePlayer(Entity* e, Entity* p, float turn) : enemy(e), player(p), turnSpeed(turn) {}
	~FacePlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
	float AngleBetween();
};