#include "pch.h"
#include "SeekAndFleePlayer.h"

void SeekAndFleePlayer::OnInitialize()
{
	currentGrid = aStarSolver->GetCurrentGrid(enemy->GetPosition());
}

void SeekAndFleePlayer::OnTerminate(Status s)
{
}

Status SeekAndFleePlayer::Update()
{
	DirectX::XMFLOAT3 playerPos = player->GetPosition();
	DirectX::XMFLOAT3 enemyPos = enemy->GetPosition();

	// Slow us down if we are moving. This enemy doesn't walk
	float distance = sqrt(pow(playerPos.x - enemyPos.x, 2) + pow(playerPos.z - enemyPos.z, 2));
	currentSpeed = enemy->GetRBody()->getLinearVelocity() * dampingScalar;
	enemy->GetRBody()->activate();
	enemy->GetRBody()->setLinearVelocity(currentSpeed);

	if (distance > minimumDistance)
	{
		//movementDirection = btVector3(playerPos.x - enemyPos.x, 0.0f, playerPos.z - enemyPos.z);
		//currentSpeed = movementSpeed;
		*inRange = false;
	}
	else
	{
		if (*cooldownTimer <= 0)
		{
			//XMFLOAT3 teleportPos;

			//XMFLOAT3 directionOffsetPos;

			//XMStoreFloat3(&directionOffsetPos, XMVectorMultiply(XMVector3Normalize(XMLoadFloat3(&player->GetDirectionVector())), XMLoadFloat3(&XMFLOAT3(-minimumDistance, 0, -minimumDistance))));

			//XMStoreFloat3(&teleportPos, XMVectorAdd(XMLoadFloat3(&playerPos), XMLoadFloat3(&directionOffsetPos)));

			//teleportPos.y = enemyPos.y;

			//enemy->SetPosition(teleportPos);

			Node* teleportNode = currentGrid->FindNearestNode(enemy->GetPosition());

			vector<Node*> possibleTeleports;
			int adjacentCount;

			currentGrid->GetUnobstructedMoves(currentGrid->FindNearestNode(player->GetPosition()), possibleTeleports, adjacentCount);

			if (!possibleTeleports.empty())
			{
				static std::random_device rd;  // Will be used to obtain a seed for the random number engine.
				static std::mt19937 gen(rd()); // Standard Mersenne twister seeded with rd().
				static std::uniform_int_distribution<> dist1(0, possibleTeleports.size() - 1);

				teleportNode = possibleTeleports[dist1(gen)];

				possibleTeleports.clear();
				currentGrid->GetUnobstructedMoves(teleportNode, possibleTeleports, adjacentCount);

				if (!possibleTeleports.empty())
				{
					static std::uniform_int_distribution<> dist2(0, possibleTeleports.size() - 1);
					teleportNode = possibleTeleports[dist2(gen)];
				}
			}

			XMFLOAT3 teleportPos = teleportNode->GetPos();
			teleportPos.y = enemyPos.y;

			enemy->SetPosition(teleportPos);
			enemy->CalcWorldMatrix();

			*cooldownTimer = maxCooldownTime;
		}
		

		*inRange = true;
	}

	// FORCES ADDED TO RIGIDBODY
	//enemy->GetRBody()->activate();
	//enemy->GetRBody()->setLinearVelocity(movementDirection.normalized() * currentSpeed);
	//cout << "Current speed: " << currentSpeed << endl;
	//cout << "Movement vector- X: " << movementDirection.getX() << " | Y: " << movementDirection.getY() << " | Z: " << movementDirection.getZ() << endl;


	return SUCCESS;
}
