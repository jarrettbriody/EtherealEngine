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
	direction.setY(0);

	enemy->SetDirectionVector(Utility::BulletVectorToFloat3(direction.normalized()));
	enemy->GetRBody()->setLinearVelocity(direction.normalized() * chargeSpeed);

	return SUCCESS;
}
