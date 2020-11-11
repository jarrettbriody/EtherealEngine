#pragma once
#include "ScriptManager.h"
#include <iostream>

using namespace std;

class Barrel : public ScriptManager
{
	string test = "Scope";

	void Init();

	void Update();
};

