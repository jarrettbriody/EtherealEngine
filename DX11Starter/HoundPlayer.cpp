#include "pch.h"
#include "HoundPlayer.h"

#define HORNED_ATTACK_VOLUME 0.5f

void HoundPlayer::OnInitialize()
{
}

void HoundPlayer::OnTerminate(Status s)
{
}

Status HoundPlayer::Update()
{
	// enemy->GetRBody()->clearForces();
	/*btVector3 direction = player->GetRBody()->getCenterOfMassPosition() - enemy->GetRBody()->getCenterOfMassPosition();
	direction.setY(0);*/

	// enemy->SetDirectionVector(Utility::BulletVectorToFloat3(direction.normalized()));

	enemy->GetRBody()->activate();
	enemy->GetRBody()->setLinearVelocity(Utility::Float3ToBulletVector(enemy->GetTransform().GetDirectionVector()).normalized() * pounceSpeed);
	//enemy->GetRBody()->applyCentralImpulse(Utility::Float3ToBulletVector(enemy->GetDirectionVector()).normalized() * pounceSpeed);

	*cooldownTimer = maxCooldownTime;

	int index = (rand() % 9);
	Config::FMODResult = Config::FMODSystem->playSound(Config::HornedAttack[index], Config::SFXGroup, false, &Config::SFXChannel);
	Config::SFXChannel->setVolume(HORNED_ATTACK_VOLUME);
	XMFLOAT3 epos = enemy->GetTransform().GetPosition();
	FMOD_VECTOR pos = { epos.x, epos.y, epos.z };
	FMOD_VECTOR vel = { 0, 0, 0 };

	Config::SFXChannel->set3DAttributes(&pos, &vel);
	Config::SFXChannel->set3DMinMaxDistance(0, 75.0f);

	return SUCCESS;
}
