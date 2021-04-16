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
	case SwordState::SetTrajectory:
		SetLerpPositions();
		break;
	case SwordState::Slashing:
		UpdateSwordPostion();
		Slash();
		break;
	case SwordState::Reset:
		ResetSword();
		break;
	case SwordState::Idle:
		UpdateSwordPostion();
		break;
	default:
		break;
	}
}

void BloodSword::UpdateSwordPostion()
{
	// Pseudo childing sword to camera
	// position sword entity relative to camera
	// multiply above into camera lookat matrix
	// ^ will have to do this every frame 
	XMFLOAT3 newSwordPos = XMFLOAT3(cam->position.x + cam->direction.x, cam->position.y, cam->position.z + cam->direction.z);
	XMFLOAT3 lerpPos;
	XMStoreFloat3(&lerpPos, XMVectorLerp(XMLoadFloat3(&entity->GetPosition()), XMLoadFloat3(&newSwordPos), deltaTime * 10.0f));

	entity->SetPosition(lerpPos);

	/*XMMATRIX swordMatrix = XMLoadFloat4x4(&sword->GetWorldMatrix());
	XMMATRIX camLookAtMatrix = XMLoadFloat4x4(&cam->GetViewMatrix());
	XMFLOAT4X4 updatedSwordMatrix;
	XMStoreFloat4x4(&updatedSwordMatrix, XMMatrixMultiply(swordMatrix, camLookAtMatrix));

	sword->SetWorldMatrix(updatedSwordMatrix);*/
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
	endPos = XMFLOAT3(cam->position.x + cam->direction.x*2, cam->position.y, cam->position.z + cam->direction.z * 2);

	// lerp from default to start pos


	// set state
	ss = SwordState::Slashing;
}

void BloodSword::Slash()
{
	// set up a collection of start and end points that make sense for the screen (cutting horizontal, vertical, diagonal) and randomly choose what to use
	

	// there is also the possiblity of using DirectX lerping
	XMFLOAT3 current;
	XMVECTOR start = XMLoadFloat3(&entity->GetPosition());
	XMVECTOR end = XMLoadFloat3(&XMFLOAT3(cam->position.x + cam->direction.x * 2, cam->position.y, cam->position.z + cam->direction.z * 2));
	XMStoreFloat3(&current, DirectX::XMVectorLerp(start, end, deltaTime * 10.0f));

	entity->SetPosition(current);

	// use the bullet lerp fuction to move from the start to the end
	// entity->SetPosition(Utility::BulletVectorToFloat3(startPos.lerp(endPos, deltaTime)));

	// TODO: if the entity reaches the end position set the state to reset
	if (Utility::CheckXMFLOAT3Equivalence(current, endPos))
	{
		ss = SwordState::Idle;
	}
}

void BloodSword::ResetSword()
{

	/*XMFLOAT3 current;
	XMVECTOR start = XMLoadFloat3(&entity->GetPosition());
	XMVECTOR end = XMLoadFloat3(&startPos);
	XMStoreFloat3(&current, DirectX::XMVectorLerp(start, end, deltaTime * 10.0f));

	entity->SetPosition(current);*/

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
