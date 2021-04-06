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
	btRigidBody* icicleRb = entity->GetRBody();
	Entity* otherE = (Entity*)other->getUserPointer();

	// cout << "Blood Icicle Hit: " << otherE->GetName().c_str() << endl;
	if (otherE->tag->c_str() == std::string("Environment"))
	{
		icicleRb->clearForces();
		icicleRb->setActivationState(0); // pin icicle to environment
	}

	// cout << "Blood Icicle Hit: " << otherE->GetName().c_str() << endl;
	if (otherE->tag->c_str() == std::string("Enemy"))
	{
		std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(otherE, 1.0f);

		if (!childEntities.empty())
		{
			Entity* closestChild = childEntities[0];
			for each (Entity* e in childEntities)
			{
				float currentClosestDistance = closestChild->GetRBody()->getCenterOfMassPosition().distance(icicleRb->getCenterOfMassPosition());
				float contenderDistance = e->GetRBody()->getCenterOfMassPosition().distance(icicleRb->getCenterOfMassPosition());

				if (currentClosestDistance > contenderDistance)
				{
					closestChild = e;
				}
			}

			// will probably need these
			// btRigidBody* closestChildRb = closestChild->GetRBody();
			// entity->AddChildEntity(closestChild);
			
			// for now just to make sure the part we hit is generally correct
			closestChild->EmptyEntity();

			// TODO: Removing rigidbody but not visual mesh to reposition the pinned part on the blood icicle and follow it as it continues flying
			// Figure out usefulness an dproper way to the the Utilty methods
			// Potential useful methods/features: GetBTCompoundShape(), EmptyEntity(), rigidbody motion states 

		}
	}
}
