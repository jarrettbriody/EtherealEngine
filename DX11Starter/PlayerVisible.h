#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"

class PlayerVisible : public Behavior
{
private:
	Entity* enemy;
	Entity* player;
public:
	PlayerVisible(Entity* e, Entity* p) : enemy(e), player(p) {}
	~PlayerVisible() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};