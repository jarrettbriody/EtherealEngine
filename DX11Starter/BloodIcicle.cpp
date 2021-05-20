#include "pch.h"
#include "BloodIcicle.h"

void BloodIcicle::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	gameManagerScript = (GameManager*)scriptFunctionsMap[(*eMap)["GameManager"]->GetName()]["GAMEMANAGER"];
	fpsControllerScript = (FPSController*)scriptFunctionsMap[(*eMap)["FPSController"]->GetName()]["FPSCONTROLLER"];

	// Do not allow the icicle to receive reaction forces
	entity->GetRBody()->setCollisionFlags(entity->GetRBody()->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE); 

	ParticleEmitterDescription emitDesc;
	emitDesc.emitterPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	emitDesc.parentName = entity->GetName();
	emitDesc.parentWorld = entity->GetTransform().GetWorldMatrixPtr();//&emitterTransform;//
	emitDesc.emitterDirection = NEG_Y_AXIS;
	emitDesc.colorCount = 1;
	ParticleColor particleColors[1] = {
		{XMFLOAT4(0.45f, 0.0f, 0.0f, 0.7f), 1.0f},
	};
	emitDesc.colors = particleColors;
	emitDesc.bakeWorldMatOnEmission = true;
	emitDesc.emissionStartRadius = 0.1f;
	emitDesc.emissionEndRadius = 0.25f;
	emitDesc.emissionRate = 50.0;
	emitDesc.maxParticles = 500;
	emitDesc.particleInitMinSpeed = 10.0f;
	emitDesc.particleInitMaxSpeed = 20.0f;
	emitDesc.particleMinLifetime = 4.0f;
	emitDesc.particleMaxLifetime = 6.0f;
	emitDesc.particleInitMinScale = 0.1f;
	emitDesc.particleInitMaxScale = 0.2f;
	//emitDesc.fadeInEndTime = 0.1f;
	//emitDesc.fadeIn = true;
	emitDesc.fadeOutStartTime = 0.5f;
	emitDesc.fadeOut = true;
	emitDesc.particleAcceleration = XMFLOAT3(0, -15.0f, 0);

	emitter = new GPUParticleEmitter(emitDesc);
}

void BloodIcicle::Update()
{
	XMStoreFloat4x4(&translation, XMMatrixTranspose(XMMatrixTranslationFromVector(XMLoadFloat3(&entity->GetTransform().GetPosition()))));
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
			((ParticleEmitter*)emitter)->SetIsActive(false);
		}

		// if this icicle hits an enemy and there is not already a body part pinned to the icicle then split the enemy mesh and give each of the child entities the tag "Body Part" to detect the next necessary collision to accurately pin a body part
		if (otherE->HasTag("Enemy") && !bodyPartPinned && !otherE->destroyed)
		{
			XMFLOAT3 epos = otherE->GetTransform().GetPosition();
			// Update the game manager attribute for enemies alive
			gameManagerScript->DecrementEnemiesAlive();

			// if an enemy is currently leashed when hit by a blood icicle reset the hookshot
			if(fpsControllerScript->GetPlayerState() == PlayerState::HookshotLeash && fpsControllerScript->GetLeashedEntity() == otherE) fpsControllerScript->ResetHookshotTransform();
			otherE->RemoveFromPhysicsSimulation();
			std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(otherE,"Body Part", "", 10.0f,30.0f,20.0f, "BODYPART");  

			for each (Entity* e in childEntities)
			{
				e->GetRBody()->activate();
				// lower values because of incoming blood projectile
				//e->GetRBody()->applyCentralImpulse(btVector3(15, 15, 15));
				//e->GetRBody()->applyTorqueImpulse(btVector3(10, 10, 10));
				e->GetRBody()->clearForces();
			}

			//gameManagerScript->AddRangeToTotalSplitMeshEntities(childEntities);

			int index = (rand() % 6);
			Config::FMODResult = Config::FMODSystem->playSound(Config::Icicle[index], Config::SFXGroup, false, &Config::SFXChannel);
			Config::SFXChannel->setVolume(ICICLE_IMPACT_VOLUME * Config::SFXVolume);
			FMOD_VECTOR pos = { epos.x, epos.y, epos.z };
			FMOD_VECTOR vel = { 0, 0, 0 };

			Config::SFXChannel->set3DAttributes(&pos, &vel);
			Config::SFXChannel->set3DMinMaxDistance(0, 75.0f);
		}

		// if this icicle hits a child entity of a recently split enemy and there is not already a body part pinned to the icicle then pin the collided body part
		if (otherE->HasTag("Body Part") && !bodyPartPinned)
		{
			closestChild = otherE;
			XMFLOAT3 epos = otherE->GetTransform().GetPosition();
			//closestChild->RemoveFromPhysicsSimulation(); //---> works better without this right now
			bodyPartPinned = true;

			btRigidBody* rb = closestChild->GetRBody();
			Config::DynamicsWorld->removeRigidBody(closestChild->GetRBody());
			btVector3 localInertia(0, 0, 0);
			rb->setMassProps(0.0f, localInertia);
			rb->activate();
			rb->clearForces();
			btVector3 zeroVector(0, 0, 0);
			rb->setLinearVelocity(zeroVector);
			rb->setAngularVelocity(zeroVector);
			rb->setAngularFactor(btVector3(0, 0, 0)); // do not allow the child to rotate after pinned TODO: Why doesn't this work
			rb->setGravity(btVector3(0, 0, 0));
			rb->setCollisionFlags(entity->GetRBody()->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);
			Config::DynamicsWorld->addRigidBody(closestChild->GetRBody());

			XMFLOAT3 closestPos = closestChild->GetTransform().GetPosition();
			XMFLOAT3 entityPos = entity->GetTransform().GetPosition();
			closestChild->GetTransform().SetParent(&translation);
			closestChild->GetTransform().SetPosition(XMFLOAT3(closestPos.x - entityPos.x, closestPos.y - entityPos.y, closestPos.z - entityPos.z));

			int index = (rand() % 6);
			Config::FMODResult = Config::FMODSystem->playSound(Config::Icicle[index], Config::SFXGroup, false, &Config::SFXChannel);
			Config::SFXChannel->setVolume(ICICLE_IMPACT_VOLUME * Config::SFXVolume);
			FMOD_VECTOR pos = { epos.x, epos.y, epos.z };
			FMOD_VECTOR vel = { 0, 0, 0 };

			Config::SFXChannel->set3DAttributes(&pos, &vel);
			Config::SFXChannel->set3DMinMaxDistance(0, 75.0f);

			/*
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
			*/

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
