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
	lerpPositionFrom = entity->GetPosition();
	lerpDirectionFrom = entity->GetDirectionVector();
	
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
	case SwordState::Reset:
		ResetState();
		break;
	default:
		break;
	}

	entity->SetPosition(finalLerpPos);	
	entity->SetDirectionVector(finalLerpDir);
}

void BloodSword::StartSlash()
{
	ss = SwordState::Raised; 
}

void BloodSword::IdleState()
{
	finalLerpPos = XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y - 2.0f /*Offset*/, cam->position.z + -cam->right.z + cam->direction.z);
	finalLerpDir = XMFLOAT3(cam->right.x, cam->right.y, cam->right.z);
}

void BloodSword::RaisedState()
{
	lerpPositionTo = XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y + 0.5f/*Offset*/, cam->position.z + -cam->right.z + cam->direction.z);

	CalcLerp();

	if (CheckTransformationsNearEqual(true, false))
	{
		ss = SwordState::Slashing;
	}
}

void BloodSword::SlashingState()
{
	lerpPositionTo = XMFLOAT3(cam->position.x + cam->direction.x * 5.0f + cam->right.x * 4.0f, cam->position.y - 4.0f/*Offset*/, cam->position.z + cam->direction.z * 2.0f + cam->right.z * 5.0f);
	lerpDirectionTo = XMFLOAT3(cam->right.x, cam->right.y + Utility::DegToRad(45), cam->right.z + Utility::DegToRad(10));

	CalcLerp();

	if (CheckTransformationsNearEqual(true, true))
	{
		ss = SwordState::Reset;
	}
	
}

void BloodSword::ResetState()
{
	lerpPositionTo = XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y - 2.0f /*Offset*/, cam->position.z + -cam->right.z + cam->direction.z);
	lerpDirectionTo = XMFLOAT3(cam->right.x, cam->right.y, cam->right.z);

	CalcLerp();

	if (CheckTransformationsNearEqual(true, true))
	{
		ss = SwordState::Idle;
	}

}

bool BloodSword::CheckTransformationsNearEqual(bool checkPos, bool checkDir)
{
	XMVECTOR lerpPosFrom = XMLoadFloat3(&lerpPositionFrom);
	XMVECTOR lerpPosTo = XMLoadFloat3(&lerpPositionTo);
	XMVECTOR lerpDirFrom = XMLoadFloat3(&lerpDirectionFrom);
	XMVECTOR lerpDirTo = XMLoadFloat3(&lerpDirectionTo);
	XMVECTOR posTolerance = XMLoadFloat3(&positionLerpTolerance);
	XMVECTOR dirTolerance = XMLoadFloat3(&directionLerpTolerance);
	
	if (checkPos && checkDir)
	{
		return XMVector3NearEqual(lerpPosFrom, lerpPosTo, posTolerance) && XMVector3NearEqual(lerpDirFrom, lerpDirTo, dirTolerance);
	}
	else if (checkPos)
	{
		return XMVector3NearEqual(lerpPosFrom, lerpPosTo, posTolerance);
	}
	else if (checkDir)
	{
		return XMVector3NearEqual(lerpDirFrom, lerpDirTo, dirTolerance);
	}
	else
	{
		return false;
	}
}

void BloodSword::CalcLerp()
{
	finalLerpPos = XMFLOAT3(Utility::FloatLerp(lerpPositionFrom.x, lerpPositionTo.x, deltaTime * positionLerpScalar), Utility::FloatLerp(lerpPositionFrom.y, lerpPositionTo.y, deltaTime * positionLerpScalar), Utility::FloatLerp(lerpPositionFrom.z, lerpPositionTo.z, deltaTime * positionLerpScalar));
	finalLerpDir = XMFLOAT3(Utility::FloatLerp(lerpDirectionFrom.x, lerpDirectionTo.x, deltaTime * directionLerpScalar), Utility::FloatLerp(lerpDirectionFrom.y, lerpDirectionTo.y, deltaTime * directionLerpScalar), Utility::FloatLerp(lerpDirectionFrom.z, lerpDirectionTo.z, deltaTime * directionLerpScalar));
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
