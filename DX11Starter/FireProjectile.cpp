#include "pch.h"
#include "FireProjectile.h"

#define CYCLOPS_ATTACK_VOLUME 0.5f

void FireProjectile::OnInitialize()
{
	projectile = SceneLoader::GetInstance()->CreateEntity(projectileParams);
	projectile->GetRBody()->setCollisionFlags(projectile->GetRBody()->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);
	projectile->GetRBody()->setGravity(btVector3(0, 0, 0));
}


void FireProjectile::OnTerminate(Status s)
{
}

Status FireProjectile::Update()
{	
	btVector3 shootingPos = enemy->GetRBody()->getCenterOfMassPosition();
	shootingPos.setY(shootingPos.getY() + 6.0f); // Offset to shoot from mouth
	projectile->GetTransform().SetPosition(Utility::BulletVectorToFloat3(shootingPos));

	btVector3 direction = player->GetRBody()->getCenterOfMassPosition() - shootingPos;
	projectile->GetRBody()->setLinearVelocity(direction.normalized() * projectileSpeed);

	*cooldownTimer = maxCooldownTime;

	if (enemy->HasTag("Cyclops")) {
		int index = (rand() % 7);
		Config::FMODResult = Config::FMODSystem->playSound(Config::CyclopsAttack[index], Config::SFXGroup, false, &Config::SFXChannel);
		Config::SFXChannel->setVolume(CYCLOPS_ATTACK_VOLUME);
		XMFLOAT3 epos = enemy->GetTransform().GetPosition();
		FMOD_VECTOR pos = { epos.x, epos.y, epos.z };
		FMOD_VECTOR vel = { 0, 0, 0 };

		Config::SFXChannel->set3DAttributes(&pos, &vel);
		Config::SFXChannel->set3DMinMaxDistance(0, 75.0f);
	}
	else if (enemy->HasTag("Tower")) {
		int index = (rand() % 9);
		Config::FMODResult = Config::FMODSystem->playSound(Config::TowerAttack[index], Config::SFXGroup, false, &Config::SFXChannel);
		Config::SFXChannel->setVolume(CYCLOPS_ATTACK_VOLUME);
		XMFLOAT3 epos = enemy->GetTransform().GetPosition();
		FMOD_VECTOR pos = { epos.x, epos.y, epos.z };
		FMOD_VECTOR vel = { 0, 0, 0 };

		Config::SFXChannel->set3DAttributes(&pos, &vel);
		Config::SFXChannel->set3DMinMaxDistance(0, 75.0f);
	}
	

	return SUCCESS;
}

// cyclops - straight shot projectile
// tower - seeking projectile 
// bull - run through
// default enemy - hound you