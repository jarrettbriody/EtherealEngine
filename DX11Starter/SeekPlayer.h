#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"

class SeekPlayer : public Behavior
{
private:
	Entity* enemy;
	Entity* player;
	float movementSpeed;
	float maxSpeed;
	float currentSpeed;
	float minimumDistance;
	float dampingScalar = 0.9f;
	btVector3 movementDirection;
	btVector3 impulseVector;
public:
	SeekPlayer(Entity* e, Entity* p, float speed, float max, float distance) : enemy(e), player(p), movementSpeed(speed), maxSpeed(max), minimumDistance(distance) {}
	~SeekPlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};