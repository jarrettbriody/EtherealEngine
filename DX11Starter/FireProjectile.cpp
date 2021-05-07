#include "pch.h"
#include "FireProjectile.h"

void FireProjectile::OnInitialize()
{
	projectile = SceneLoader::GetInstance()->CreateEntity(projectileParams);
	projectile->GetRBody()->setCollisionFlags(projectile->GetRBody()->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);

}


void FireProjectile::OnTerminate(Status s)
{
}

Status FireProjectile::Update()
{	
	btVector3 shootingPos = enemy->GetRBody()->getCenterOfMassPosition();
	shootingPos.setY(shootingPos.getY() + 8.0f); // Offset to shoot from eye

	btVector3 direction = player->GetRBody()->getCenterOfMassPosition() - shootingPos;

	projectile->SetDirectionVector(Utility::BulletVectorToFloat3(direction.normalized()));
	projectile->SetPosition(Utility::BulletVectorToFloat3(shootingPos));

	projectile->GetRBody()->setGravity(btVector3(0, 0, 0));

	projectile->GetRBody()->activate();
	projectile->GetRBody()->setAngularFactor(btVector3(0, 0, 1)); // Constrain rotations on x and y axes
	projectile->GetRBody()->setLinearVelocity(direction.normalized() * projectileSpeed);

	return SUCCESS;
}

// cyclops - straight shot projectile
// tower - seeking projectile 
// bull - run through
// default enemy - hound you