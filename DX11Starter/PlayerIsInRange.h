#pragma once
#include "pch.h"
#include "BehaviorTree.h"

class PlayerIsInRange : public Behavior
{
private:
	bool* inRange;
public:
	PlayerIsInRange(bool* isPlayerInRange) : inRange(isPlayerInRange) {}
	~PlayerIsInRange() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

