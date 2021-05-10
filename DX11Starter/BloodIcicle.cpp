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
	//if (bodyPartPinned)
	//{
	//	closestChild->GetTransform().SetPosition(entity->GetTransform().GetPosition());
	//}
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
				//e->GetRBody()->applyCentralImpulse(btVector3(15, 15, 15));
				//e->GetRBody()->applyTorqueImpulse(btVector3(10, 10, 10));
				e->GetRBody()->clearForces();
			}

			gameManagerScript->AddRangeToTotalSplitMeshEntities(childEntities);
		}

		// if this icicle hits a child entity of a recently split enemy and there is not already a body part pinned to the icicle then pin the collided body part
		if (otherE->HasTag("Body Part"))
		{
			closestChild = otherE;

			//closestChild->RemoveFromPhysicsSimulation(); //---> works better without this right now
			bodyPartPinned = true;

			//closestChild->GetRBody()->activate();
			closestChild->GetRBody()->clearForces();
			closestChild->GetRBody()->setAngularFactor(btVector3(0, 0, 0)); // do not allow the child to rotate after pinned TODO: Why doesn't this work
			closestChild->GetRBody()->setGravity(btVector3(0, 0, 0));
			closestChild->GetRBody()->setCollisionFlags(entity->GetRBody()->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);

			XMFLOAT3 closestPos = closestChild->GetTransform().GetPosition();
			XMFLOAT3 entityPos = entity->GetTransform().GetPosition();
			XMFLOAT4 closestRot = closestChild->GetTransform().GetRotationQuaternion();
			XMFLOAT4 entityRot = entity->GetTransform().GetRotationQuaternion();
			XMFLOAT3 closestScl = closestChild->GetTransform().GetScale();
			XMFLOAT3 entityScl = entity->GetTransform().GetScale();
			closestChild->GetTransform().SetPosition(XMFLOAT3(closestPos.x - entityPos.x, closestPos.y - entityPos.y, closestPos.z - entityPos.z));

			XMMATRIX rot = XMMatrixRotationQuaternion(XMLoadFloat4(&entityRot));
			rot = XMMatrixMultiply(XMMatrixRotationQuaternion(XMLoadFloat4(&closestRot)),XMMatrixTranspose(rot));
			XMFLOAT4 newQuat;
			XMStoreFloat4(&newQuat, XMQuaternionRotationMatrix(rot));
			closestChild->GetTransform().SetRotationQuaternion(newQuat);

			XMFLOAT3 newScl = XMFLOAT3(closestScl.x * (1.0f / entityScl.x), closestScl.y * (1.0f / entityScl.y * 2.0f), closestScl.z * (1.0f / entityScl.z));
			closestChild->GetTransform().SetScale(newScl);

			entity->GetTransformPtr()->AddChild(closestChild->GetTransformPtr(), true);

			/*
			btTransform trans;
			trans.setIdentity();
			trans.setOrigin(Utility::Float3ToBulletVector(closestChild->GetTransform().GetPosition())); 
			
			XMFLOAT4 rot = closestChild->GetTransform().GetRotationQuaternion();

			btQuaternion res = btQuaternion(rot.x, rot.y, rot.z, rot.w);
			trans.setRotation(res);

			XMFLOAT3 scale = closestChild->GetTransform().GetScale();
			btVector3 scl = Utility::Float3ToBulletVector(scale);
			closestChild->GetRBody()->getCollisionShape()->setLocalScaling(scl);

			closestChild->GetRBody()->setCenterOfMassTransform(trans);
			*/
		}
	}
}
