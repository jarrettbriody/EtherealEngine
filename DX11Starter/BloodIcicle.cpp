#include "pch.h"
#include "BloodIcicle.h"

void BloodIcicle::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	gameManagerScript = (GameManager*)scriptFunctionsMap[(*eMap)["GameManager"]->GetName()]["GAMEMANAGER"];
	fpsControllerScript = (FPSController*)scriptFunctionsMap[(*eMap)["FPSController"]->GetName()]["FPSCONTROLLER"];

	// Do not allow the icicle to receive reaction forces
	entity->GetRBody()->setCollisionFlags(entity->GetRBody()->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE); 
}

void BloodIcicle::Update()
{
	if (bodyPartPinned)
	{
		closestChild->GetTransform().SetPosition(entity->GetTransform().GetPosition());
	}
}

void BloodIcicle::OnCollision(btCollisionObject* other)
{
	btRigidBody* icicleRb = entity->GetRBody();
	PhysicsWrapper* wrapper = (PhysicsWrapper*)other->getUserPointer();

	if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY)
	{
		Entity* otherE = (Entity*)wrapper->objectPointer;

		// cout << "Blood Icicle Hit: " << otherE->GetName().c_str() << endl;

		// pin icicle to environment
		if (otherE->HasTag("Environment"))
		{
			icicleRb->clearForces();
			icicleRb->setActivationState(0); 
		}

		// if this icicle hits an enemy and there is not already a body part pinned to the icicle then split the enemy mesh and give each of the child entities the tag "Body Part" to detect the next necessary collision to accurately pin a body part
		if (otherE->HasTag("Enemy") && !bodyPartPinned)
		{
			// Update the game manager attribute for enemies alive
			gameManagerScript->DecrementEnemiesAlive();

			// if an enemy is currently leashed when hit by a blood icicle reset the hookshot
			if(fpsControllerScript->GetPlayerState() == PlayerState::HookshotLeash && fpsControllerScript->GetLeashedEntity() == otherE) fpsControllerScript->ResetHookshotTransform();

			std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(otherE, 1.0f);  

			for each (Entity* e in childEntities)
			{
				e->AddTag("Body Part");

				e->GetRBody()->activate();
				// lower values because of incoming blood projectile
				e->GetRBody()->applyCentralImpulse(btVector3(15, 15, 15));
				e->GetRBody()->applyTorqueImpulse(btVector3(10, 10, 10));
			}

			gameManagerScript->AddRangeToTotalSplitMeshEntities(childEntities);
		}

		// if this icicle hits a child entity of a recently split enemy and there is not already a body part pinned to the icicle then pin the collided body part
		if (otherE->HasTag("Body Part") && !bodyPartPinned)
		{
			closestChild = otherE;

			//closestChild->RemoveFromPhysicsSimulation(); ---> works better without this right now
			bodyPartPinned = true;
			closestChild->GetRBody()->activate();
			closestChild->GetRBody()->clearForces();
			closestChild->GetRBody()->setAngularFactor(btVector3(0, 0, 0)); // do not allow the child to rotate after pinned TODO: Why doesn't this work
		}
	}
}
