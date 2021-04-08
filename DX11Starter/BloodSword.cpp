#include "pch.h"
#include "BloodSword.h"

void BloodSword::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	// set default pos

	ss = SwordState::Idle;
}

void BloodSword::Update()
{
	switch (ss)
	{
	case SwordState::SetTrajectory:
		SetLerpPositions();
		break;
	case SwordState::Slashing:
		Slash();
		break;
	case SwordState::Reset:
		ResetSword();
		break;
	case SwordState::Idle:
		break;
	default:
		break;
	}
}

void BloodSword::StartSwing()
{
	ss = SwordState::SetTrajectory; 
}

void BloodSword::SetLerpPositions()
{
	// set start and end pos

	// set state
	ss = SwordState::Slashing;
}

void BloodSword::Slash()
{
	// set up a collection of start and end points that make sense for the screen (cutting horizontal, vertical, diagonal) and randomly choose what to use
	

	// there is also the possiblity of using DirectX lerping

	// use the bullet lerp fuction to move from the start to the end
	entity->SetPosition(Utility::BulletVectorToFloat3(startPos.lerp(endPos, deltaTime)));

	// if the entity reaches the end positoin set the state to reset
	// if()
}

void BloodSword::ResetSword()
{
	entity->SetPosition(Utility::BulletVectorToFloat3(defaultPos)); // lerp back? 

	ss = SwordState::Idle;
}

void BloodSword::OnCollision(btCollisionObject* other)
{
	Entity* otherE = (Entity*)other->getUserPointer();

	// std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(otherE, 1.0f);


	// cout << "Blood Sword Hit: " << otherE->GetName().c_str() << endl;
}
