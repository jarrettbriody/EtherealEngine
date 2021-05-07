#include "pch.h"
#include "FireProjectile.h"

void FireProjectile::OnInitialize()
{
	
}


void FireProjectile::OnTerminate(Status s)
{
}

Status FireProjectile::Update()
{	
	btVector3 direction = player->GetRBody()->getCenterOfMassPosition() - enemy->GetRBody()->getCenterOfMassPosition(); // TODO: Reverse this when models are turned to face correctly

	Entity* projectile = SceneLoader::GetInstance()->CreateEntity(projectileParams);
	
	XMFLOAT3 pos = enemy->GetPosition();

	projectile->SetPosition(pos);

	projectile->GetRBody()->setGravity(btVector3(0, 0, 0));

	// Do not allow the icicle to receive reaction forces
	projectile->GetRBody()->activate();
	projectile->GetRBody()->applyCentralImpulse(direction.normalized() * projectileSpeed);

	return SUCCESS;
}

// cyclops - straight shot projectile
// tower - seeking projectile 
// bull - run through
// default enemy - hound you