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
	entity->SetPosition(XMFLOAT3(cam->position.x + cam->right.x + cam->direction.x, cam->position.y, cam->position.z + cam->right.z + cam->direction.z));
	entity->SetRotation(XMFLOAT3(cam->xRotation + 1.5708f /*90 degrees*/, cam->yRotation, cam->zRotation));
}

void BloodSword::Slash()
{
	// Potenntially able to set up a collection of start and end points that make sense for the screen (cutting horizontal, vertical, diagonal) and randomly choose what to use
	
	XMFLOAT3 current;
	XMVECTOR start = XMLoadFloat3(&entity->GetPosition());
	XMVECTOR end = XMLoadFloat3(&XMFLOAT3(cam->position.x + -cam->right.x + cam->direction.x, cam->position.y, cam->position.z + -cam->right.z + cam->direction.z));
	XMStoreFloat3(&current, DirectX::XMVectorLerp(start, end, deltaTime * lerpScalar));

	entity->SetPosition(current);

	bool lerpEndPointReached = XMVector3Equal(XMLoadFloat3(&current), end);

	// TODO: doesn't seem to correctly switch states once end point is reached
	if (lerpEndPointReached)
	{
		ss = SwordState::Idle;
	}

	// could use the bullet lerp fuction to move from the start to the end
	// entity->SetPosition(Utility::BulletVectorToFloat3(startPos.lerp(endPos, deltaTime)));
}

void BloodSword::OnCollision(btCollisionObject* other)
{
	PhysicsWrapper* wrapper = (PhysicsWrapper*)other->getUserPointer();

	if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY)
	{
		Entity* otherE = (Entity*)wrapper->objectPointer;

		// cout << "Blood Sword Hit: " << otherE->GetName().c_str() << endl;
		
		// std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(otherE, 1.0f);
	}
}
