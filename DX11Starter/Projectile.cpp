#include "pch.h"
#include "Projectile.h"

void Projectile::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
}

void Projectile::Update()
{
}

void Projectile::OnCollision(btCollisionObject* other)
{
	PhysicsWrapper* wrapper = (PhysicsWrapper*)other->getUserPointer();

	if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY)
	{
		Entity* otherE = (Entity*)wrapper->objectPointer;

		// entity->Destroy(); // destroy projectile when it hits anything
	}
}
