#pragma once
#include "pch.h"
#include "BehaviorTree.h"

class InCombat : public Behavior
{
private:
    bool* inCombat;
public:
	InCombat(bool* combat) : inCombat(combat) {}
	~InCombat() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};