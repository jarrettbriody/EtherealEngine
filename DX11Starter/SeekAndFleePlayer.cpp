#include "pch.h"
#include "SeekAndFleePlayer.h"

void SeekAndFleePlayer::OnInitialize()
{
}

void SeekAndFleePlayer::OnTerminate(Status s)
{
}

Status SeekAndFleePlayer::Update()
{
	DirectX::XMFLOAT3 playerPos = player->GetPosition();
	DirectX::XMFLOAT3 enemyPos = enemy->GetPosition();

	float distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.z - enemyPos.z, 2));

	if (distance > minimumDistance)
	{
		movementDirection = btVector3(playerPos.x - enemyPos.x, 0.0f, playerPos.z - enemyPos.z);
		currentSpeed = movementSpeed;
		*inRange = false;
	}
	else
	{
		XMFLOAT3 teleportPos;

		XMFLOAT3 directionOffsetPos;

		XMStoreFloat3(&directionOffsetPos, XMVectorMultiply(XMVector3Normalize(XMLoadFloat3(&player->GetDirectionVector())), XMLoadFloat3(&XMFLOAT3(-minimumDistance, 0, -minimumDistance))));

		XMStoreFloat3(&teleportPos, XMVectorAdd(XMLoadFloat3(&playerPos), XMLoadFloat3(&directionOffsetPos)));

		teleportPos.y = enemyPos.y;

		enemy->SetPosition(teleportPos);

		*inRange = true;
	}

	// FORCES ADDED TO RIGIDBODY
	enemy->GetRBody()->activate();
	enemy->GetRBody()->setLinearVelocity(movementDirection.normalized() * currentSpeed);
	//cout << "Current speed: " << currentSpeed << endl;
	//cout << "Movement vector- X: " << movementDirection.getX() << " | Y: " << movementDirection.getY() << " | Z: " << movementDirection.getZ() << endl;


	return SUCCESS;
}
