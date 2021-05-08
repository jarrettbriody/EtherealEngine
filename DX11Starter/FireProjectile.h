#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"
#include "SceneLoader.h"

class FireProjectile : public Behavior
{
private:
	Entity* player;
	Entity* enemy;
	EntityCreationParameters projectileParams;
	float projectileSpeed;
	float* cooldownTimer;
	float maxCooldownTime;

	Entity* projectile;
public:
	FireProjectile(Entity* e, Entity* p, EntityCreationParameters projParams, float pSpeed, float* cdTime, float maxCdTime) : enemy(e), player(p), projectileParams(projParams), projectileSpeed(pSpeed), cooldownTimer(cdTime), maxCooldownTime(maxCdTime) {}
	~FireProjectile() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

