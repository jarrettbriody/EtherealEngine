#pragma once
#include "ScriptManager.h"
#include "BehaviorTreeBuilder.h"
#include "EnemySeesPlayer.h"

class EnemyTest : public ScriptManager
{
	map<string, Entity*>* eMap;

	BehaviorTree* bt;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);
};
