#include "pch.h"
#include "BloodIcicle.h"

void BloodIcicle::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
	initialImpulse = entity->GetRBody()->getLinearVelocity();
	/*entity->GetRBody()->setAngularFactor(btVector3(0, 0, 0));
	entity->GetRBody()->setLinearFactor(btVector3(0, 0, 1));*/
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

	if (otherE->tag->c_str() == std::string("Enemy"))
	{
		// icicleRb->clearForces();

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
			
			closestChild->RemoveFromPhysicsSimulation();

			entity->AddChildEntity(closestChild); // TODO: Figure out how to maintain scale, continue force and correctly position
			closestChild->SetPosition(XMFLOAT3(0, 0, 0));  

			// icicleRb->applyCentralImpulse(initialImpulse);

			// Potential useful methods/features: GetBTCompoundShape(), EmptyEntity(), rigidbody motion states 

		}
	}
}
