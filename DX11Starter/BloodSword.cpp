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
	ss = SwordState::Slashing; 
}

void BloodSword::UpdateSwordTransform()
{
	entity->SetPosition(XMFLOAT3(cam->position.x + (cam->right.x*4) + cam->direction.x, cam->position.y, cam->position.z + (cam->right.z*4) + cam->direction.z));
	entity->SetRotation(XMFLOAT3(cam->xRotation + (1.5708f) /*90 degrees*/, cam->yRotation, cam->zRotation));
}

void BloodSword::Slash()
{
	// Potentially able to set up a collection of start and end points that make sense for the screen (cutting horizontal, vertical, diagonal) and randomly choose what to use
	
	XMFLOAT3 current;
	XMVECTOR start = XMLoadFloat3(&entity->GetPosition());
	XMVECTOR end = XMLoadFloat3(&XMFLOAT3(cam->position.x + (-cam->right.x*4) + cam->direction.x, cam->position.y, cam->position.z + (-cam->right.z*4) + cam->direction.z));
	XMStoreFloat3(&current, DirectX::XMVectorLerp(start, end, deltaTime * lerpScalar));

	entity->SetPosition(current);

	bool lerpEndPointReached = XMVector3NearEqual(XMLoadFloat3(&current), end, XMLoadFloat3(&lerpTolerance));

	if (lerpEndPointReached)
	{
		ss = SwordState::Reset;
	}
}

void BloodSword::ResetSword()
{
	XMFLOAT3 current;
	XMVECTOR start = XMLoadFloat3(&entity->GetPosition());
	XMVECTOR end = XMLoadFloat3(&XMFLOAT3(cam->position.x + cam->right.x + cam->direction.x, cam->position.y, cam->position.z + cam->right.z + cam->direction.z));
	XMStoreFloat3(&current, DirectX::XMVectorLerp(start, end, deltaTime * lerpScalar));

	entity->SetPosition(current);

	bool lerpEndPointReached = XMVector3NearEqual(XMLoadFloat3(&current), end, XMLoadFloat3(&lerpTolerance));

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
