#pragma once
#include "pch.h"
#include "BehaviorTree.h"
#include "Entity.h"

class FireProjectile : public Behavior
{
private:
	Entity* player;
	Entity* enemy;
	Entity* projectile;
	float projectileSpeed;
	float deltaTime;
public:
	FireProjectile(Entity* e, Entity* p, Entity* proj, float pSpeed, float dTime) : enemy(e), player(p), projectile(proj), projectileSpeed(pSpeed), deltaTime(dTime) {}
	~FireProjectile() {}

	void OnInitialize();
	void OnTerminate(Status s);
	Status Update();
};

