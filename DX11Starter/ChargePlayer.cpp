#include "pch.h"
#include "ChargePlayer.h"

#define BULL_ATTACK_VOLUME 0.75f

void ChargePlayer::OnInitialize()
{
}

void ChargePlayer::OnTerminate(Status s)
{
}

Status ChargePlayer::Update()
{
	btVector3 direction = player->GetRBody()->getCenterOfMassPosition() - enemy->GetRBody()->getCenterOfMassPosition();
	direction.setY(0);

	enemy->GetTransform().SetDirectionVector(Utility::BulletVectorToFloat3(direction.normalized()));
	enemy->GetRBody()->setLinearVelocity(direction.normalized() * chargeSpeed);

	bool isPlaying = false;
	if(BullSFXChannel != nullptr)
		BullSFXChannel->isPlaying(&isPlaying);
	if (!isPlaying) {
		int index = (rand() % 7);
		Config::FMODResult = Config::FMODSystem->playSound(Config::BullAttack[index], Config::SFXGroup, false, &BullSFXChannel);
		BullSFXChannel->setVolume(BULL_ATTACK_VOLUME);
		XMFLOAT3 epos = enemy->GetTransform().GetPosition();
		FMOD_VECTOR pos = { epos.x, epos.y, epos.z };
		FMOD_VECTOR vel = { 0, 0, 0 };

		Config::SFXChannel->set3DAttributes(&pos, &vel);
		Config::SFXChannel->set3DMinMaxDistance(0, 75.0f);
	}
	

	return SUCCESS;
}
