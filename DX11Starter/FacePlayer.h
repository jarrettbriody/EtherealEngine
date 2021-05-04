#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"

class FacePlayer : public Behavior
{
private:
	Entity* enemy;
	Entity* player;
	float speed;
	float* deltaTime;
public:
	FacePlayer(Entity* e, Entity* p, float speed, float* deltaTime) : enemy(e), player(p), speed(speed), deltaTime(deltaTime) {}
	~FacePlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};