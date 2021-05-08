#pragma once
#include "pch.h"
#include "BehaviorTree.h"

class AbilityAvailable : public Behavior
{
private:
	float* coolDownTimer;
	
public:
	AbilityAvailable(float* cdTimer) : coolDownTimer(cdTimer) {}
	~AbilityAvailable() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

