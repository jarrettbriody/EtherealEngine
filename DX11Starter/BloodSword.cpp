#include "pch.h"
#include "BloodSword.h" 

void BloodSword::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	gameManager = (*eMap)["GameManager"];

	cam = ScriptManager::EERenderer->GetCamera("main");

	keyboard = Keyboard::GetInstance();

	ss = SwordState::Idle;
	
	// starting positioning
	entity->SetParentWorldMatrix(cam->GetWorldMatrixPtr());
	
	finalLerpPos = XMFLOAT3(3, -1, 3);
	lerpPositionFrom = finalLerpPos; 

	finalLerpRot = XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f);
	lerpRotationFrom = finalLerpRot;

	entity->SetPosition(lerpPositionFrom); 
	entity->SetRotation(lerpRotationFrom);
	entity->SetScale(0.4f, 0.4f, 0.4f);
	entity->CalcWorldMatrix();

	/*slashPointsRight = {
		XMFLOAT3(5, 0, 3),
		XMFLOAT3(4, 0.5, 4),
		XMFLOAT3(3, -1.5, 4.25),
		XMFLOAT3(2, -2, 4.5),
		XMFLOAT3(1, -2.5, 4.5),
		XMFLOAT3(0, -3, 4.5),
		XMFLOAT3(-2, -3.5, 4.5),
		XMFLOAT3(-4, -4, 4.25),
		XMFLOAT3(-6, -4.5, 4),
		XMFLOAT3(-7, -5, 3),
		XMFLOAT3(-8, -6, 3)
	};

	slashPointsLeft = {
		XMFLOAT3(-5, 0, 3),
		XMFLOAT3(-4, 0.5, 4),
		XMFLOAT3(-3, -1.5, 4.25),
		XMFLOAT3(-2, -2, 4.5),
		XMFLOAT3(-1, -2.5, 4.5),
		XMFLOAT3(0, -3, 4.5),
		XMFLOAT3(2, -3.5, 4.5),
		XMFLOAT3(4, -4, 4.25),
		XMFLOAT3(6, -4.5, 4),
		XMFLOAT3(7, -5, 3),
		XMFLOAT3(8, -6, 3)
	};*/

	slashPointsRight = GenerateSlashPoints(XMFLOAT3(5, 3, 3), XMFLOAT3(-8, -4, 3), 0.5, 5.0f);
	slashPointsLeft = GenerateSlashPoints(XMFLOAT3(-5, 3, 3), XMFLOAT3(8, -4, 3), 0.5, 5.0f);
}

void BloodSword::Update()
{
	// Get the current lerp in relative to the local space of the camera
	switch (ss)
	{
	case SwordState::Idle:
		IdleState();
		break;
	case SwordState::Raised:
		RaisedState();
		break;
	case SwordState::Slashing:
		SlashingState();
		break;
	case SwordState::Reset:
		ResetState();
		break;
	default:
		break;
	}
	
	// Update the lerpFromPosition for the next time the lerp calculation is made 
	lerpPositionFrom = finalLerpPos;
	lerpRotationFrom = finalLerpRot;
	
	// Update blood sword position with local coordinate lerping data relative to camera 
	entity->SetRotation(finalLerpRot);
	entity->SetPosition(finalLerpPos);
	

	// Calc world matrix of the sword
	cam->CalcViewMatrix();
	cam->CalcWorldMatrix();
	entity->CalcWorldMatrix();
	//cam->CalcWorldMatrix(); // Putting camera world matrix calc after the entity makes the sword jitter much less severe...not sure why?
}

void BloodSword::StartSlash()
{
	if (ss == SwordState::Idle) // Only allow slash if it is idle state
	{
		if (keyboard->KeyIsPressed(0x41)) // a - left -> since character is right handed we only have to check for this, otherwise do the right handed slash
		{
			slashPoints = slashPointsLeft;
			raisedRotation = XMFLOAT3(XMConvertToRadians(45.0f), XMConvertToRadians(-90.0f), 0.0f);
			slashRotation = XMFLOAT3(XMConvertToRadians(45.0f), XMConvertToRadians(-90.0f), XMConvertToRadians(-70.0f));
		}
		else
		{
			slashPoints = slashPointsRight;
			raisedRotation = XMFLOAT3(XMConvertToRadians(-45.0f), XMConvertToRadians(-90.0f), 0.0f);
			slashRotation = XMFLOAT3(XMConvertToRadians(-45.0f), XMConvertToRadians(-90.0f), XMConvertToRadians(-70.0f));;
		}

		ss = SwordState::Raised;
	}
}

void BloodSword::IdleState()
{
	// not doing any lerping in the idle state
	finalLerpPos = XMFLOAT3(3, -1, 3); // starting pos
	finalLerpRot = XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f);
}

void BloodSword::RaisedState()
{
	lerpPositionTo = slashPoints[slashPointsIndex]; // First index is the raised state
	lerpRotationTo = raisedRotation;
	
	CalcLerp();

	cout << "Raising" << endl;

	if (CheckTransformationsNearEqual(true, true))
	{
		slashPointsIndex++;
		CheckSwordSlashHit();
		ss = SwordState::Slashing;
	}
}

void BloodSword::SlashingState()
{
	lerpPositionTo = slashPoints[slashPointsIndex];
	lerpRotationTo = slashRotation;
	
	CalcLerp();

	cout << "Slashing" << endl;

	if (CheckTransformationsNearEqual(true, false))
	{
		slashPointsIndex++;

		if (slashPointsIndex == slashPoints.size() && CheckTransformationsNearEqual(true, true))
		{
			slashPointsIndex = 0;
			ss = SwordState::Reset;
		}
		
	}
}

void BloodSword::ResetState()
{
	lerpPositionTo = XMFLOAT3(3, -1, 3);
	lerpRotationTo = XMFLOAT3(0, XMConvertToRadians(-90.0f), 0.0f);

	CalcLerp();

	cout << "Resetting" << endl;

	if (CheckTransformationsNearEqual(true, true))
	{
		ss = SwordState::Idle;
	}
}

bool BloodSword::CheckTransformationsNearEqual(bool checkPos, bool checkRot)
{
	XMVECTOR lerpPosFrom = XMLoadFloat3(&lerpPositionFrom);
	XMVECTOR lerpPosTo = XMLoadFloat3(&lerpPositionTo);
	XMVECTOR posTolerance = XMLoadFloat3(&positionLerpTolerance);

	XMVECTOR lerpRotFrom = XMLoadFloat3(&lerpRotationFrom);
	XMVECTOR lerpRotTo = XMLoadFloat3(&lerpRotationTo);
	XMVECTOR rotTolerance = XMLoadFloat3(&rotationLerpTolerance);

	if (checkPos)
	{
		return XMVector3NearEqual(lerpPosFrom, lerpPosTo, posTolerance);
	}
	else if (checkRot)
	{
		return XMVector3NearEqual(lerpRotFrom, lerpRotTo, rotTolerance);
	}
	else
	{
		return XMVector3NearEqual(lerpPosFrom, lerpPosTo, posTolerance) && XMVector3NearEqual(lerpRotFrom, lerpRotTo, rotTolerance);
	}

	
}

void BloodSword::CalcLerp() 
{
	float posLerpScalar;
	float rotLerpScalar;

	if (ss == SwordState::Raised || ss == SwordState::Reset)
	{
		posLerpScalar = readyingPositionLerpScalar;
		rotLerpScalar = readyingRotationLerpScalar;
	}
	else
	{
		posLerpScalar = slashPositionLerpScalar;
		rotLerpScalar = slashRotationLerpScalar;
	}

	XMStoreFloat3(&finalLerpPos, XMVectorLerp(XMLoadFloat3(&lerpPositionFrom), XMLoadFloat3(&lerpPositionTo), deltaTime * posLerpScalar));
	XMStoreFloat3(&finalLerpRot, XMVectorLerp(XMLoadFloat3(&lerpRotationFrom), XMLoadFloat3(&lerpRotationTo), deltaTime * rotLerpScalar));
}

std::vector<XMFLOAT3> BloodSword::GenerateSlashPoints(XMFLOAT3 startingPos, XMFLOAT3 endingPos, float interval, float maxZ)
{
	std::vector<XMFLOAT3> generatedSlashPoints;
	XMFLOAT3 newPos = startingPos;
	generatedSlashPoints.push_back(newPos);

	bool leftSlash = newPos.x < 0; // if the starting x position is less than zero then we are slashing from the left

	// loop through and increment/decrement new points accordingly (x goes down or up depending on the slash direction, y always goes down, z increases until x is line with camera and then goes down)
	// TODO: Only caring about the x and y works for now, but for future work would be better to consider all coordinates (the problem is the z start and end position should be around the same because you are slashing out and in)
	while (!XMVector3NearEqual(XMLoadFloat3(&XMFLOAT3(newPos.x, newPos.y, 0)), XMLoadFloat3((&XMFLOAT3(endingPos.x, endingPos.y, 0))), XMLoadFloat3(&positionLerpTolerance))) 
	{
		float x = newPos.x;
		float y = newPos.y;
		float z = newPos.z;
		y -= interval;
		y = max(y, endingPos.y); // don't go beyond ending y
		
		if (leftSlash)
		{
			x += interval;
			x = min(x, endingPos.x); // don't go beyond ending x

			if (newPos.x >= 1) // we want to make sure that the max z is kept for a couple lerp points so this is >= 1 rather than >= 0
			{
				z -= interval;
			}
			else
			{
				z += interval;

				z = min(z, maxZ); // don't go beyond ending x
			}
		}
		else
		{
			x -= interval;
			x = max(x, endingPos.x); // don't go beyond ending x

			if (newPos.x <= -1) // we want to make sure that the max z is kept for a couple lerp points so this is <= -1 rather than <= 0
			{
				z -= interval;
			}
			else
			{
				z += interval;

				z = min(z, maxZ); // don't go beyond max z
			}
		}										    

		newPos = XMFLOAT3(x, y, z);
		generatedSlashPoints.push_back(newPos);
	}

	return generatedSlashPoints;
}

void BloodSword::CheckSwordSlashHit()
{
	float currentAngle = atan2(cam->direction.z, cam->direction.x);
	float halfViewAngle = XMConvertToRadians(viewAngle / 2);
	float hypotenuse = viewDistance / cos(halfViewAngle);
	XMFLOAT3 viewLeft = XMFLOAT3(cos(currentAngle + halfViewAngle) * hypotenuse, 0.0f, sin(currentAngle + halfViewAngle) * hypotenuse);
	XMFLOAT3 viewRight = XMFLOAT3(cos(currentAngle - halfViewAngle) * hypotenuse, 0.0f, sin(currentAngle - halfViewAngle) * hypotenuse);

	XMFLOAT3 viewFront;
	XMStoreFloat3(&viewFront, XMVectorSubtract(XMLoadFloat3(&viewRight), XMLoadFloat3(&viewLeft)));

	std::vector<Entity*> enemies = EESceneLoader->sceneEntitiesTagMap["Enemy"];

	for each (Entity* enemy in enemies)
	{
		XMFLOAT3 playerPos =  Utility::BulletVectorToFloat3(eMap->find("FPSController")->second->GetRBody()->getCenterOfMassPosition());
		XMFLOAT3 enemyPos = Utility::BulletVectorToFloat3(enemy->GetRBody()->getCenterOfMassPosition());

		XMFLOAT3 flooredPos = XMFLOAT3(playerPos.x, 0, playerPos.z);
		XMFLOAT3 flooredEnemyPos = XMFLOAT3(enemyPos.x, 0, enemyPos.z);

		XMFLOAT3 triVertToEnemy;
		XMStoreFloat3(&triVertToEnemy, XMVectorSubtract(XMLoadFloat3(&flooredEnemyPos), XMLoadFloat3(&flooredPos)));

		XMFLOAT3 perpendicular = XMFLOAT3(viewLeft.z, viewLeft.y, -viewLeft.x);

		float dotProduct = 0;
		XMStoreFloat(&dotProduct, XMVector3Dot(XMLoadFloat3(&triVertToEnemy), XMLoadFloat3(&perpendicular)));

		if (dotProduct > 0)
		{
			XMStoreFloat3(&triVertToEnemy,XMVectorSubtract(XMLoadFloat3(&flooredEnemyPos), XMVectorAdd(XMLoadFloat3(&flooredPos), XMLoadFloat3(&viewLeft))));
			perpendicular = XMFLOAT3(viewFront.z, viewFront.y, -viewFront.x);

			XMStoreFloat(&dotProduct, XMVector3Dot(XMLoadFloat3(&triVertToEnemy), XMLoadFloat3(&perpendicular)));

			if (dotProduct > 0)
			{
				XMStoreFloat3(&triVertToEnemy, XMVectorSubtract(XMLoadFloat3(&flooredEnemyPos), XMVectorAdd(XMLoadFloat3(&flooredPos), XMLoadFloat3(&viewRight))));
				perpendicular = XMFLOAT3(-viewRight.z, -viewRight.y, viewRight.x);

				XMStoreFloat(&dotProduct, XMVector3Dot(XMLoadFloat3(&triVertToEnemy), XMLoadFloat3(&perpendicular)));

				if (dotProduct > 0)
				{
					// Update the game manager attribute for enemies alive
					GameManager* gameManagerScript = (GameManager*)scriptFunctionsMap[gameManager->GetName()]["GAMEMANAGER"];
					gameManagerScript->DecrementEnemiesAlive();

					// enemy is in the triangle, split it apart
					std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(enemy, 10.0f);

					for each (Entity * e in childEntities)
					{
						e->tag = std::string("Body Part");

						/*std::vector<Entity*>::iterator itr = std::find(enemies.begin(), enemies.end(), e);

						if (itr != enemies.cend())
						{
							enemies.erase(enemies.begin() + std::distance(enemies.begin(), itr));
						}*/
					}
				}

			}
		}
	}
}


void BloodSword::OnCollision(btCollisionObject* other)
{
	PhysicsWrapper* wrapper = (PhysicsWrapper*)other->getUserPointer();

	if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY)
	{
		Entity* otherE = (Entity*)wrapper->objectPointer;

		// cout << "Blood Sword Hit: " << otherE->GetName().c_str() << endl;
		
		if (otherE->tag.STDStr() == std::string("Enemy"))
		{
			std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(otherE, 1.0f);
		}
	}
}
