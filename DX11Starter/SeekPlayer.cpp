#include "pch.h"
#include "SeekPlayer.h"

void SeekPlayer::OnInitialize()
{
}

void SeekPlayer::OnTerminate(Status s)
{
}

Status SeekPlayer::Update()
{
	DirectX::XMFLOAT3 playerPos = player->GetPosition();
	DirectX::XMFLOAT3 enemyPos = enemy->GetPosition();

	float distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.z - enemyPos.z, 2));

	if (distance > minimumDistance)
	{
		movementDirection = btVector3(playerPos.x - enemyPos.x, 0.0f, playerPos.z - enemyPos.z);
		currentSpeed = movementSpeed;
	}
	else
		currentSpeed = enemy->GetRBody()->getLinearVelocity().length() * dampingScalar;

	//cout << "Current speed: " << currentSpeed << endl;
	//cout << "Movement vector- X: " << movementDirection.getX() << " | Y: " << movementDirection.getY() << " | Z: " << movementDirection.getZ() << endl;

	// FORCES ADDED TO RIGIDBODY
	enemy->GetRBody()->activate();
	enemy->GetRBody()->setLinearVelocity(movementDirection.normalized() * currentSpeed);
	//enemy->GetRBody()->applyCentralImpulse(impulseVector);

	return SUCCESS;
}