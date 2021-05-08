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
	enemy->GetRBody()->clearForces();
	enemy->GetRBody()->applyCentralImpulse(Utility::Float3ToBulletVector(enemy->GetDirectionVector()).normalized() * pounceSpeed);

	return SUCCESS;
}
