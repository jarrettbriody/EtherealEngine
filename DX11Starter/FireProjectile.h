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

	Entity* projectile;
public:
	FireProjectile(Entity* e, Entity* p, EntityCreationParameters projParams, float pSpeed) : enemy(e), player(p), projectileParams(projParams), projectileSpeed(pSpeed) {}
	~FireProjectile() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

