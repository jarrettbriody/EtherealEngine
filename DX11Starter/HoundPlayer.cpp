#include "pch.h"
#include "HoundPlayer.h"

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
	enemy->GetRBody()->setLinearVelocity(Utility::Float3ToBulletVector(enemy->GetDirectionVector()).normalized() * pounceSpeed);
	//enemy->GetRBody()->applyCentralImpulse(Utility::Float3ToBulletVector(enemy->GetDirectionVector()).normalized() * pounceSpeed);

	*cooldownTimer = maxCooldownTime;

	return SUCCESS;
}
