#include "pch.h"
#include "BloodSword.h"

void BloodSword::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	cam = ScriptManager::EERenderer->GetCamera("main");

	ss = SwordState::Idle;

	// default positions
	//entity->SetPosition(XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y - 2.0f/*Offset*/, cam->position.z + -cam->right.z + cam->direction.z));
	//entity->SetRotation(XMFLOAT3(0, cam->yRotation + Utility::DegToRad(-90), cam->zRotation));
	//entity->SetDirectionVector(cam->direction);
}

void BloodSword::Update()
{
	switch (ss)
	{
	case SwordState::Idle:
		IdleState();
		break;
	case SwordState::Raised:
		RaisedState();
		break;
	case SwordState::Slashing:
		SlashingState();
		break;
	default:
		break;
	}

	XMVECTOR lerpPosFrom = XMLoadFloat3(&lerpPositionFrom);
	XMVECTOR lerpPosTo = XMLoadFloat3(&lerpPositionTo);
	XMVECTOR lerpRotFrom = XMLoadFloat3(&lerpRotationFrom);
	XMVECTOR lerpRotTo = XMLoadFloat3(&lerpRotationTo);
	XMVECTOR lerpDirFrom = XMLoadFloat3(&lerpDirectionFrom);
	XMVECTOR lerpDirTo = XMLoadFloat3(&lerpDirectionTo);
	XMVECTOR posTolerance = XMLoadFloat3(&positionLerpTolerance);
	XMVECTOR rotTolerance = XMLoadFloat3(&rotationLerpTolerance);
	XMVECTOR dirTolerance = XMLoadFloat3(&directionLerpTolerance);

	negligentTransformationChange = XMVector3NearEqual(lerpPosFrom, lerpPosTo, posTolerance) /*&& XMVector3NearEqual(lerpRotFrom, lerpRotTo, rotTolerance)*/ && XMVector3NearEqual(lerpDirFrom, lerpDirTo, dirTolerance);

	if (!negligentTransformationChange /*|| ss != SwordState::Idle*/)
	{
		lerpPositionFrom = entity->GetPosition();
		lerpRotationFrom = entity->GetEulerAngles();
		lerpDirectionFrom = entity->GetDirectionVector();
		entity->SetPosition(XMFLOAT3(Utility::FloatLerp(lerpPositionFrom.x, lerpPositionTo.x, deltaTime * positionLerpScalar), Utility::FloatLerp(lerpPositionFrom.y, lerpPositionTo.y, deltaTime * positionLerpScalar), Utility::FloatLerp(lerpPositionFrom.z, lerpPositionTo.z, deltaTime * positionLerpScalar)));
		// entity->SetRotation(XMFLOAT3(Utility::FloatLerp(lerpRotationFrom.x, lerpRotationTo.x, deltaTime * rotationLerpScalar), Utility::FloatLerp(lerpRotationFrom.y, lerpRotationTo.y, deltaTime * rotationLerpScalar), Utility::FloatLerp(lerpRotationFrom.z, lerpRotationTo.z, deltaTime * rotationLerpScalar)));
		entity->SetDirectionVector(XMFLOAT3(Utility::FloatLerp(lerpDirectionFrom.x, lerpDirectionTo.x, deltaTime * directionLerpScalar), Utility::FloatLerp(lerpDirectionFrom.y, lerpDirectionTo.y, deltaTime * directionLerpScalar), Utility::FloatLerp(lerpDirectionFrom.z, lerpDirectionTo.z, deltaTime * directionLerpScalar)));
	}
}

void BloodSword::StartSlash()
{
	ss = SwordState::Raised; 
}

void BloodSword::IdleState()
{
	lerpPositionTo = XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y - 2.0f /*Offset*/, cam->position.z + -cam->right.z + cam->direction.z);
	lerpRotationTo = XMFLOAT3(cam->xRotation, cam->yRotation + Utility::DegToRad(90), cam->zRotation);
	lerpDirectionTo = XMFLOAT3(cam->right.x, cam->right.y, cam->right.z);
}

void BloodSword::RaisedState()
{
	lerpPositionTo = XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y + 0.5f/*Offset*/, cam->position.z + -cam->right.z + cam->direction.z);

	if (negligentTransformationChange)
	{
		negligentTransformationChange = false;
		ss = SwordState::Slashing;
	}
}

void BloodSword::SlashingState()
{
	lerpPositionTo = XMFLOAT3(cam->position.x + cam->right.x*4.0f + cam->direction.x, cam->position.y - 4.0f/*Offset*/, cam->position.z + cam->right.z*4.0f + cam->direction.z);

	if (negligentTransformationChange)
	{
		negligentTransformationChange = false;
		ss = SwordState::Idle;
	}
	
}

void BloodSword::ResetSword()
{
	// TODO: Is this needed? 
	XMFLOAT3 endPos = XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y - 2.0f/*Offset*/, cam->position.z + -cam->right.z + cam->direction.z);
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
