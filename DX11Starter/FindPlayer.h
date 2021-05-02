#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"
#include "Grid.h"

class FindPlayer : public Behavior
{
private:
	Grid* grid;
	std::list<Node*>* path;
	Entity* enemy;
	Entity* player;
public:
	FindPlayer(Entity* e, Entity* p, Grid* grid, std::list<Node*>* path) : enemy(e), player(p), grid(grid), path(path) {}
	~FindPlayer() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};