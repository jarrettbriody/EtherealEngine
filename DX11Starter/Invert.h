#pragma once
#include "pch.h"
#include "BehaviorTree.h"

class Invert : public Decorator
{
public:
	Invert(Behavior* child) : Decorator(child) {}
	~Invert() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};