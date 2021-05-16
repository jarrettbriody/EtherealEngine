#include "pch.h"
#include "SeekAndFleePlayer.h"

void SeekAndFleePlayer::OnInitialize()
{
	currentGrid = aStarSolver->GetCurrentGrid(enemy->GetTransform().GetPosition());
}

void SeekAndFleePlayer::OnTerminate(Status s)
{
}

Status SeekAndFleePlayer::Update()
{
	DirectX::XMFLOAT3 playerPos = player->GetTransform().GetPosition();
	DirectX::XMFLOAT3 enemyPos = enemy->GetTransform().GetPosition();

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

			//Node* teleportNode = currentGrid->FindNearestNode(enemy->GetTransform().GetPosition());

			Grid* cGrid = NavmeshHandler::GetInstance()->GetGridAtPosition(player->GetTransform().GetPosition());

			Node* teleportNode = cGrid->FindNearestNode(player->GetTransform().GetPosition());

			vector<Node*> possibleTeleports;
			int adjacentCount;

			cGrid->GetUnobstructedMoves(teleportNode, possibleTeleports, adjacentCount);

			if (possibleTeleports.size() > 0)
			{
				static std::uniform_int_distribution<> dist1(0, possibleTeleports.size() - 1);

				teleportNode = possibleTeleports[rand() % possibleTeleports.size()];

				possibleTeleports.clear();
				cGrid = NavmeshHandler::GetInstance()->GetGridAtPosition(teleportNode->GetPos());
				cGrid->GetUnobstructedMoves(teleportNode, possibleTeleports, adjacentCount);

				if (!possibleTeleports.empty())
				{
					teleportNode = possibleTeleports[rand() % possibleTeleports.size()];
				}
			}
			else {
				cGrid = NavmeshHandler::GetInstance()->GetGridAtPosition(enemy->GetTransform().GetPosition());
				teleportNode = cGrid->FindNearestNode(player->GetTransform().GetPosition());
				cGrid->GetUnobstructedMoves(teleportNode, possibleTeleports, adjacentCount);
				if (possibleTeleports.size() > 0) {
					teleportNode = possibleTeleports[rand() % possibleTeleports.size()];
				}
			}

			if (possibleTeleports.size() > 0) {
				XMFLOAT3 teleportPos = teleportNode->GetPos();
				teleportPos.y = enemyPos.y;

				enemy->GetTransform().SetPosition(teleportPos);

				*cooldownTimer = maxCooldownTime;
			}
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
