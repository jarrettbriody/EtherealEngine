#include "pch.h"
#include "ChargePlayer.h"

void ChargePlayer::OnInitialize()
{
}

void ChargePlayer::OnTerminate(Status s)
{
}

Status ChargePlayer::Update()
{
	btVector3 direction = player->GetRBody()->getCenterOfMassPosition() - enemy->GetRBody()->getCenterOfMassPosition(); // TODO: Reverse this when models are turned to face correctly

	enemy->GetRBody()->applyCentralImpulse(direction.normalized() * chargeSpeed);

	return SUCCESS;
}
