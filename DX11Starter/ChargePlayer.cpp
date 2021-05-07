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
	btVector3 direction = player->GetRBody()->getCenterOfMassPosition() - enemy->GetRBody()->getCenterOfMassPosition();

	enemy->GetRBody()->setLinearVelocity(direction.normalized() * chargeSpeed);

	return SUCCESS;
}
