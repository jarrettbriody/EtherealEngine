#include "pch.h"
#include "BloodIcicle.h"

void BloodIcicle::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
}

void BloodIcicle::Update()
{
	
}

void BloodIcicle::OnCollision(btCollisionObject* other)
{
	Entity* otherE = (Entity*)other->getUserPointer();

	// cout << "Blood Icicle Hit: " << otherE->GetName().c_str() << endl;
	if (otherE->tag->c_str() == std::string("Environment"))
	{
		btRigidBody* icicleRb = entity->GetRBody();
		icicleRb->clearForces();
		icicleRb->setActivationState(0); // pin icicle to environment
	}
}
