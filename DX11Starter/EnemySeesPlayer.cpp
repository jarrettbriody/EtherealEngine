#include "pch.h"
#include "EnemySeesPlayer.h"

void EnemySeesPlayer::OnInitialize()
{
}

void EnemySeesPlayer::OnTerminate(Status s)
{
}

Status EnemySeesPlayer::Update()
{
	//cout << "Player Position= X: " << player->GetPosition().x << " | Y: " << player->GetPosition().y << " | Z: " << player->GetPosition().z << endl;

	// TODO: Add check that returns FAILURE if the player status stops them from being seen (i.e. Death)

	//enemy->RotateAroundAxis(Y_AXIS, 0.05f);

	// Base values for calculating the vision cone
	XMFLOAT3 forward = enemy->GetTransform().GetDirectionVector();
	float currentAngle = atan2(forward.z, forward.x);
	float halfViewAngle = (XM_PI / 180.0f) * viewAngle / 2.0f;
	float hypotenuse = viewDistance / cos(halfViewAngle);

	// Our vision cone vectors
	XMVECTOR viewLeft = XMVectorScale(XMVectorSet(cos(currentAngle + halfViewAngle), 0.0f, sin(currentAngle + halfViewAngle), 0.0f), hypotenuse);
	XMVECTOR viewRight = XMVectorScale(XMVectorSet(cos(currentAngle - halfViewAngle), 0.0f, sin(currentAngle - halfViewAngle), 0.0f), hypotenuse);
	XMVECTOR viewFront = XMVectorSubtract(viewRight, viewLeft);

	//------
	// Debug line shit
	//------

	//// Create the world matrix for the debug line
	//XMFLOAT4X4 wm;
	//XMStoreFloat4x4(&wm, XMMatrixTranspose(DirectX::XMMatrixIdentity()));

	//XMFLOAT3 start = enemy->GetPosition();
	//XMFLOAT3 p1 = XMFLOAT3(start.x + XMVectorGetX(viewLeft), start.y + XMVectorGetY(viewLeft), start.z + XMVectorGetZ(viewLeft));
	//XMFLOAT3 p2 = XMFLOAT3(start.x + XMVectorGetX(viewRight), start.y + XMVectorGetY(viewRight), start.z + XMVectorGetZ(viewRight));

	//// Create debug line
	//DebugLines* dl = new DebugLines("TestRay", 0, false);
	//XMFLOAT3 c = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//dl->color = c;
	//dl->worldMatrix = wm;

	//// Draw the debug line to show the raycast
	//XMFLOAT3* rayPoints = new XMFLOAT3[8];
	//rayPoints[0] = start;
	//rayPoints[1] = p1;
	//rayPoints[2] = p2;
	//rayPoints[3] = start;
	//rayPoints[4] = p1;
	//rayPoints[5] = p2;
	//rayPoints[6] = start;
	//rayPoints[7] = p1;
	//dl->GenerateCuboidVertexBuffer(rayPoints, 8);
	//delete[] rayPoints;
	
	//------
	// Back to vison calculations
	//------

	// Get the floored (y = 0) positions of both the enemy and player
	XMFLOAT3 pos = Utility::BulletVectorToFloat3(enemy->GetRBody()->getCenterOfMassPosition()); //enemy->GetPosition();
	pos.y = 0.0f;
	XMVECTOR flooredEnemyPos = XMLoadFloat3(&pos);

	pos = Utility::BulletVectorToFloat3(player->GetRBody()->getCenterOfMassPosition()); //player->GetPosition();
	pos.y = 0.0f;
	XMVECTOR flooredPlayerPos = XMLoadFloat3(&pos);

	// Get vector of enemy to player and the perpendicular to the vector we are testing
	XMVECTOR triVertToPlayer = XMVectorSubtract(flooredPlayerPos, flooredEnemyPos);
	XMVECTOR perpendicular = XMVectorSet(XMVectorGetZ(viewLeft), XMVectorGetY(viewLeft), -XMVectorGetX(viewLeft), 0.0f);

	// Checks to see if the player's position is within our vision cone
	if (XMVectorGetX(XMVector3Dot(triVertToPlayer, perpendicular)) > 0.0f)
	{
		// Adjust perspective to a new corner of the triangle and test again
		triVertToPlayer = XMVectorSubtract(flooredPlayerPos, XMVectorAdd(flooredEnemyPos, viewLeft));
		perpendicular = XMVectorSet(XMVectorGetZ(viewFront), XMVectorGetY(viewFront), -XMVectorGetX(viewFront), 0.0f);
		if (XMVectorGetX(XMVector3Dot(triVertToPlayer, perpendicular)) > 0.0f)
		{
			triVertToPlayer = XMVectorSubtract(flooredPlayerPos, XMVectorAdd(flooredEnemyPos, viewRight));
			perpendicular = XMVectorSet(-XMVectorGetZ(viewRight), -XMVectorGetY(viewRight), XMVectorGetX(viewRight), 0.0f);
			// If we pass this next check then the player is in all sides of the vision cone
			if (XMVectorGetX(XMVector3Dot(triVertToPlayer, perpendicular)) > 0.0f)
			{
				// Use a raycast to see if there is anything obstructing the enemy's view of the player

				// Update physics
				Config::DynamicsWorld->updateAabbs();
				Config::DynamicsWorld->computeOverlappingPairs();

				btVector3 from(enemy->GetTransform().GetPosition().x, enemy->GetTransform().GetPosition().y, enemy->GetTransform().GetPosition().z);
				btVector3 to(player->GetTransform().GetPosition().x, player->GetTransform().GetPosition().y, player->GetTransform().GetPosition().z);

				// Create variable to store the ray hit and set flags
				btCollisionWorld::ClosestRayResultCallback closestResult(from, to);
				closestResult.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

				Config::DynamicsWorld->rayTest(from, to, closestResult); // Raycast

				if (closestResult.hasHit())
				{
					PhysicsWrapper* wrapper = (PhysicsWrapper*)closestResult.m_collisionObject->getUserPointer();

					if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
						Entity* hit = (Entity*)wrapper->objectPointer;
						//printf("Raycast Hit: %s\n", hit->GetName().c_str());

						if (hit->GetName().c_str() == std::string("FPSController"))
						{
							*inCombat = true;
							return SUCCESS;
						}
							
					}
				}
			}
		}
	}

	return FAILURE;
}