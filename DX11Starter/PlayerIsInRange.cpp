#include "pch.h"
#include "PlayerIsInRange.h"

void PlayerIsInRange::OnInitialize()
{
}

void PlayerIsInRange::OnTerminate(Status s)
{
}

Status PlayerIsInRange::Update()
{
	DirectX::XMFLOAT3 playerPos = player->GetTransform().GetPosition();
	DirectX::XMFLOAT3 enemyPos = enemy->GetTransform().GetPosition();

	float distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.z - enemyPos.z, 2));

	if (distance <= minimumDistance)
	{
		*inRange = true;
		return SUCCESS;
	}

	*inRange = false;
	return FAILURE;
}
