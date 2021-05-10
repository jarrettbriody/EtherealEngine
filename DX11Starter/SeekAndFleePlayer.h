#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"
#include "Grid.h"
#include "AStarSolver.h"
#include <random>

class SeekAndFleePlayer : public Behavior
{
private:
	Entity* enemy;
	Entity* player;
	AStarSolver* aStarSolver;
	Grid* currentGrid;
	float* cooldownTimer;
	float maxCooldownTime;
	//float movementSpeed;
	//float maxSpeed;
	btVector3 currentSpeed;
	float minimumDistance;
	float dampingScalar = 0.9f;
	//btVector3 movementDirection;
	//btVector3 impulseVector;
	bool* inRange;
public:
	SeekAndFleePlayer(Entity* e, Entity* p, AStarSolver* aStarSolver, float* cdTime, float maxCdTime, float distance, bool* isPlayerInRange) : enemy(e), player(p), aStarSolver(aStarSolver), cooldownTimer(cdTime), maxCooldownTime(maxCdTime), minimumDistance(distance), inRange(isPlayerInRange) {}
	~SeekAndFleePlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

