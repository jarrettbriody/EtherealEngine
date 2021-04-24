#include "pch.h"
#include "BloodSword.h" 

void BloodSword::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	cam = ScriptManager::EERenderer->GetCamera("main");

	keyboard = Keyboard::GetInstance();

	ss = SwordState::Idle;
	
	// starting positioning
	entity->SetParentWorldMatrix(cam->GetWorldMatrixPtr());
	
	finalLerpPos = XMFLOAT3(3, -3, 3);
	lerpPositionFrom = finalLerpPos; 

	finalLerpRot = XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f);
	lerpRotationFrom = finalLerpRot;

	entity->SetPosition(lerpPositionFrom); 
	entity->SetRotation(lerpRotationFrom);
	entity->SetScale(0.4f, 0.4f, 0.4f);
	entity->CalcWorldMatrix();

	slashPointsRight = {
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
	};
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
	entity->CalcWorldMatrix();
	cam->CalcWorldMatrix(); // Putting camera world matrix calc after the entity makes the sword jitter much less severe...not sure why?
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
	finalLerpPos = XMFLOAT3(3, -3, 3); // starting pos
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
	lerpPositionTo = XMFLOAT3(3, -3, 3);
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
		posLerpScalar = positionLerpScalar * 0.15;
		rotLerpScalar = rotationLerpScalar * 0.15;
	}
	else
	{
		posLerpScalar = positionLerpScalar;
		rotLerpScalar = rotationLerpScalar;
	}

	XMStoreFloat3(&finalLerpPos, XMVectorLerp(XMLoadFloat3(&lerpPositionFrom), XMLoadFloat3(&lerpPositionTo), deltaTime * posLerpScalar));
	XMStoreFloat3(&finalLerpRot, XMVectorLerp(XMLoadFloat3(&lerpRotationFrom), XMLoadFloat3(&lerpRotationTo), deltaTime * rotLerpScalar));
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
