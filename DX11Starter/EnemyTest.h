#pragma once
#include "ScriptManager.h"
#include "BehaviorTreeBuilder.h"
#include "EnemySeesPlayer.h"
#include "InCombat.h"
#include "PlayerVisible.h"
#include "FacePlayer.h"
#include "SeekPlayer.h"
#include "FPSController.h"

class EnemyTest : public ScriptManager
{
	map<string, Entity*>* eMap;
	map<string, vector<ScriptManager*>> sMap;

	Grid* grid;

	XMFLOAT3 pos;
	bool inCombat = false;
	float movementSpeed = 7.5f;
	float maxSpeed = 25.0f;
	float dampingScalar = 0.09f;
	float turnSpeed = 1.0f;
	float minimumDistance = 10.0f;
	float visionConeAngle = 30.0f;
	float visionConeDistance = 30.0f;
	float oscillationMagnitude = 0.001f;
	float totalTime = 0.0f;

	BehaviorTree* bt;

	Keyboard* keyboard;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);
};
