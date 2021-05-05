#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Grid.h"
#include "Entity.h"

class FollowPath : public Behavior
{
private:
	std::vector<Node*>* path;
	Entity* enemy;
	float movementSpeed;
	float minimumDistance;
	float dampingScalar = 0.9f;
	float turningSpeed;
	double* deltaTime;
	btVector3 movementDirection;
public:
	FollowPath(std::vector<Node*>* path, Entity* e, float speed, float distance, float turn, double* delta) : path(path), enemy(e), movementSpeed(speed), minimumDistance(distance), turningSpeed(turn), deltaTime(delta) {}
	~FollowPath() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};