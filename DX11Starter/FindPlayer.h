#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"
#include "Grid.h"
#include "AStarSolver.h"

class FindPlayer : public Behavior
{
private:
	AStarSolver* aStarSolver;
	std::vector<Node*>* path;
	Entity* enemy;
	Entity* player;
public:
	FindPlayer(Entity* e, Entity* p, AStarSolver* aStarSolver, std::vector<Node*>* path) : enemy(e), player(p), aStarSolver(aStarSolver), path(path) {}
	~FindPlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};