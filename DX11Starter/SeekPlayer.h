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
	bool* inRange;
public:
	SeekPlayer(Entity* e, Entity* p, float speed, float max, float distance, bool* isPlayerInRange) : enemy(e), player(p), movementSpeed(speed), maxSpeed(max), minimumDistance(distance), inRange(isPlayerInRange) {}
	~SeekPlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};