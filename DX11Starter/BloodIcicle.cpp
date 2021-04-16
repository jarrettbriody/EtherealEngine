#include "pch.h"
#include "BloodIcicle.h"

void BloodIcicle::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
	
	// Do not allow the icicle to receive reaction forces
	entity->GetRBody()->setCollisionFlags(entity->GetRBody()->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE); 
}

void BloodIcicle::Update()
{
	if (bodyPartPinned)
	{
		closestChild->SetPosition(entity->GetPosition());
	}
}

void BloodIcicle::OnCollision(btCollisionObject* other)
{
	btRigidBody* icicleRb = entity->GetRBody();
	//Entity* otherE = (Entity*)other->getUserPointer();
	PhysicsWrapper* wrapper = (PhysicsWrapper*)other->getUserPointer();

	if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY)
	{
		Entity* otherE = (Entity*)wrapper->objectPointer;

		// cout << "Blood Icicle Hit: " << otherE->GetName().c_str() << endl;
		if (otherE->tag.STDStr() == std::string("Environment"))
		{
			icicleRb->clearForces();
			icicleRb->setActivationState(0); // pin icicle to environment
		}

		if (otherE->tag.STDStr() == std::string("Enemy"))
		{
			// TODO: Can I set the tag of the children and get a collision call back from them

			std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(otherE, 1.0f);

			if (!childEntities.empty())
			{
				closestChild = childEntities[0];
				for each (Entity * e in childEntities)
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

				//closestChild->RemoveFromPhysicsSimulation();
				bodyPartPinned = true;


				// pull out scale from icicle, and get inverse scale and multiply that into the child; 
				//XMFLOAT4X4 icicleScaleMatrix;
				//XMFLOAT4X4 adjustedChildMatrix;
				//XMStoreFloat4x4(&icicleScaleMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&entity->GetWorldMatrix())))); // TODO: How to put the icicle scale into a matrix
				//XMStoreFloat4x4(&adjustedChildMatrix, XMMatrixMultiply(XMLoadFloat4x4(&icicleScaleMatrix), XMLoadFloat4x4(&closestChild->GetWorldMatrix())));

				//entity->AddChildEntity(closestChild, adjustedChildMatrix); // TODO: Figure out how to maintain scale, correctly position
				//closestChild->SetPosition(XMFLOAT3(0, 0, 0));
			}
		}
	}
}
