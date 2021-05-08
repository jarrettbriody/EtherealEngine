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
	Entity* projectile = SceneLoader::GetInstance()->CreateEntity(projectileParams);
	projectile->GetRBody()->setCollisionFlags(projectile->GetRBody()->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);
	projectile->GetRBody()->setGravity(btVector3(0, 0, 0));

	btVector3 shootingPos = enemy->GetRBody()->getCenterOfMassPosition();
	shootingPos.setY(shootingPos.getY() + 8.0f); // Offset to shoot from eye
	projectile->SetPosition(Utility::BulletVectorToFloat3(shootingPos));

	btVector3 direction = player->GetRBody()->getCenterOfMassPosition() - shootingPos;
	projectile->GetRBody()->applyCentralImpulse(direction.normalized() * projectileSpeed);

	return SUCCESS;
}

// cyclops - straight shot projectile
// tower - seeking projectile 
// bull - run through
// default enemy - hound you