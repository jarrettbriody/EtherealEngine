#include "pch.h"
#include "BloodSword.h"

void BloodSword::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	cam = ScriptManager::EERenderer->GetCamera("main");

	ss = SwordState::Idle;
}

void BloodSword::Update()
{
	switch (ss)
	{
	case SwordState::Raised:
		Raise();
		break;
	case SwordState::Slashing:
		Slash();
		break;
	case SwordState::Reset:
		ResetSword();
	case SwordState::Idle:
		UpdateSwordTransform();
		break;
	default:
		break;
	}
}

void BloodSword::StartSlash()
{
	ss = SwordState::Raised; 
}

void BloodSword::UpdateSwordTransform()
{
	XMFLOAT3 defaultPos = XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y - 2.0f/*Offset*/, cam->position.z + -cam->right.z + cam->direction.z);
	XMFLOAT3 defaultRot = XMFLOAT3(0, cam->yRotation + Utility::DegToRad(90), cam->zRotation);

	entity->SetPosition(defaultPos);
	entity->SetRotation(defaultRot);
}

void BloodSword::Raise()
{
	// Potentially able to set up a collection of start and end points that make sense for the screen (cutting horizontal, vertical, diagonal) and randomly choose what to use
	XMFLOAT3 endPos = XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y + 0.5f/*Offset*/, cam->position.z + -cam->right.z + cam->direction.z);
	bool lerpEndPointReached = entity->LerpPositionFromTo(entity->GetPosition(), endPos, positionLerpTolerance, deltaTime, positionLerpScalar);

	if (lerpEndPointReached)
	{
		// entity->RotateAroundAxis(Z_AXIS, 1.5708f);

		// TODO: cam->zRotation works better than entity->GetEuelerAngles()
		bool lerpEndRotationReached = entity->LerpRotationFromTo(Z_AXIS, cam->zRotation, Utility::DegToRad(45), rotationLerpTolerance, deltaTime, rotationLerpScalar);
		
		if(lerpEndRotationReached) ss = SwordState::Slashing;
	}
}

void BloodSword::Slash()
{
	// Potentially able to set up a collection of start and end points that make sense for the screen (cutting horizontal, vertical, diagonal) and randomly choose what to use
	XMFLOAT3 endPos = XMFLOAT3(cam->position.x + cam->right.x*4.0f + cam->direction.x, cam->position.y - 2.0f/*Offset*/, cam->position.z + cam->right.z*4.0f + cam->direction.z);
	bool lerpEndPointReached = entity->LerpPositionFromTo(entity->GetPosition(), endPos, positionLerpTolerance, deltaTime, positionLerpScalar);

	if (lerpEndPointReached)
	{
		ss = SwordState::Idle;
	}
}

void BloodSword::ResetSword()
{
	XMFLOAT3 endPos = XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y - 2.0f/*Offset*/, cam->position.z + -cam->right.z + cam->direction.z);
	bool lerpEndPointReached = entity->LerpPositionFromTo(entity->GetPosition(), endPos, positionLerpTolerance, deltaTime, positionLerpScalar);

	if (lerpEndPointReached)
	{
		ss = SwordState::Idle;
	}
}

void BloodSword::OnCollision(btCollisionObject* other)
{
	PhysicsWrapper* wrapper = (PhysicsWrapper*)other->getUserPointer();

	if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY)
	{
		Entity* otherE = (Entity*)wrapper->objectPointer;

		// cout << "Blood Sword Hit: " << otherE->GetName().c_str() << endl;
		
		if (otherE->tag.STDStr() == std::string("Enemy"))
		{
			std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(otherE, 1.0f);
		}
	}
}
