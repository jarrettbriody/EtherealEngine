#include "pch.h"
#include "BloodSword.h"

void BloodSword::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

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

void BloodSword::StartSlash()
{
	ss = SwordState::SetTrajectory; 
}

void BloodSword::SetLerpPositions()
{
	// set start, end, and default pos (do multiple slash angles later after confirming basic functionality )
	// defaultPos = entity->GetPosition();
	startPos = entity->GetPosition();
	endPos = XMFLOAT3(startPos.x + 10, startPos.y + 10, startPos.z + 10);

	// lerp from default to start pos


	// set state
	ss = SwordState::Slashing;
}

void BloodSword::Slash()
{
	// set up a collection of start and end points that make sense for the screen (cutting horizontal, vertical, diagonal) and randomly choose what to use
	

	// there is also the possiblity of using DirectX lerping
	XMFLOAT3 current;
	XMVECTOR start = XMLoadFloat3(&startPos);
	XMVECTOR end = XMLoadFloat3(&endPos);
	XMStoreFloat3(&current, DirectX::XMVectorLerp(start, end, deltaTime * 10.0f));

	entity->SetPosition(current);

	// use the bullet lerp fuction to move from the start to the end
	// entity->SetPosition(Utility::BulletVectorToFloat3(startPos.lerp(endPos, deltaTime)));

	// TODO: if the entity reaches the end positoin set the state to reset
	// if()
}

void BloodSword::ResetSword()
{

	XMFLOAT3 current;
	XMVECTOR start = XMLoadFloat3(&entity->GetPosition());
	XMVECTOR end = XMLoadFloat3(&startPos);
	XMStoreFloat3(&current, DirectX::XMVectorLerp(start, end, deltaTime));

	entity->SetPosition(current);

	// entity->SetPosition(Utility::BulletVectorToFloat3(defaultPos)); // lerp back? 



	ss = SwordState::Idle;
}

void BloodSword::OnCollision(btCollisionObject* other)
{
	//Entity* otherE = (Entity*)other->getUserPointer();

	PhysicsWrapper* wrapper = (PhysicsWrapper*)other->getUserPointer();

	if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY)
	{
		Entity* otherE = (Entity*)wrapper->objectPointer;

		// cout << "Blood Sword Hit: " << otherE->GetName().c_str() << endl;
		
		// std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(otherE, 1.0f);


	}
}
