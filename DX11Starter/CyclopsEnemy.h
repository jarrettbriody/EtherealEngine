#pragma once
#include "ScriptManager.h"
#include "BehaviorTreeBuilder.h"
#include "EnemySeesPlayer.h"
#include "InCombat.h"
#include "PlayerVisible.h"
#include "FacePlayer.h"
#include "SeekPlayer.h"
#include "FindPlayer.h"
#include "FollowPath.h"
#include "Idle.h"
#include "FireProjectile.h"
#include "PlayerIsInRange.h"
#include "AbilityAvailable.h"
#include "FPSController.h"
#include "NavmeshHandler.h"
#include "AStarSolver.h"

class CyclopsEnemy : public ScriptManager, public BaseEnemy
{
	~CyclopsEnemy();

	map<string, Entity*>* eMap;
	map<string, vector<ScriptManager*>> sMap;

	FPSController* fpsControllerScript;

	//Grid* grid;
	std::vector<Node*> path;
	NavmeshHandler* navmesh = nullptr;
	AStarSolver aStarSolver;

	XMFLOAT3 pos;
	bool inCombat = false;
	bool playerIsInRange = false;
	float movementSpeed = 20.0f;
	float maxSpeed = 40.0f;
	float dampingScalar = 0.09f;
	float turnSpeed = 8.0f;
	float minimumDistance = 40.0f;
	float visionConeAngle = 50.0f;
	float visionConeDistance = 80.0f;
	float oscillationMagnitude = 0.01f;
	float totalTime = 0.0f;
	float projectileSpeed = 100.0f;
	float projectileCooldownTimer = 0.0f;
	const float PROJECTILE_COOLDOWN_MAX = 0.5f;

	bool leashed;
	float delay;

	BehaviorTree* bt;

	Keyboard* keyboard;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);

public:
	void IsLeashed(bool leashed, float delay);
	void CheckPlayerState();
};

