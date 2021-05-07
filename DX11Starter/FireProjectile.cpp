#include "pch.h"
#include "FireProjectile.h"

void FireProjectile::OnInitialize()
{
	projectile = SceneLoader::GetInstance()->CreateEntity(projectileParams);
}


void FireProjectile::OnTerminate(Status s)
{
}

Status FireProjectile::Update()
{	
	btVector3 direction = player->GetRBody()->getCenterOfMassPosition() - enemy->GetRBody()->getCenterOfMassPosition(); // TODO: Reverse this when models are turned to face correctly

	XMFLOAT3 pos = enemy->GetPosition();

	projectile->SetPosition(pos);

	projectile->GetRBody()->setGravity(btVector3(0, 0, 0));

	projectile->GetRBody()->activate();
	projectile->GetRBody()->setAngularFactor(btVector3(0, 0, 1)); // Constrain rotations on x and y axes
	projectile->GetRBody()->applyCentralImpulse(direction.normalized() * projectileSpeed);

	return SUCCESS;
}

// cyclops - straight shot projectile
// tower - seeking projectile 
// bull - run through
// default enemy - hound you