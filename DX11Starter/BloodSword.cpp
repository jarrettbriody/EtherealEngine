#include "pch.h"
#include "BloodSword.h"
	
// TODO: Lerping offset -> XMFLOAT3 newPos = XMFLAOT3(camPos.x + camDir.x + lerpOffest, etc...)
// TODO: camervaViewMatrix * swordWorldMatrix = puts it in space to camera -> set this to new blood sword every frame, need to calc world matrix (every frame!) whenever setting this 

void BloodSword::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	cam = ScriptManager::EERenderer->GetCamera("main");

	ss = SwordState::Idle;
	
	// starting positioning
	entity->SetParentWorldMatrix(cam->GetWorldMatrixPtr());
	finalLerpPos = XMFLOAT3(3, -3, 3);
	lerpPositionFrom = finalLerpPos; 
	entity->SetPosition(lerpPositionFrom); 
	entity->SetRotation(0.0f, XMConvertToRadians(180.0f), 0.0f);
	entity->SetScale(0.4f, 0.4f, 0.4f);
	entity->CalcWorldMatrix();
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
	
	// Update blood sword position with local coordinate lerping data relative to camera 
	entity->SetPosition(finalLerpPos);
	cam->CalcWorldMatrix();
	// Calc world matrix of the sword
	entity->CalcWorldMatrix();

	// Multiply the blood sword matrix into the camera view matrix so it turns the the local coordinates we lerped into world coordinates relative to the camera space
	//XMFLOAT4X4 swordMatrix;
	// Get the camera view matrix 
	//XMFLOAT4X4 camWorld = cam->GetWorldMatrix();
	//XMMATRIX camParentMatrix = XMMatrixTranspose(XMLoadFloat4x4(&camWorld));
	//XMMATRIX swordLocalMatrix = XMMatrixTranspose(XMLoadFloat4x4(&entity->GetWorldMatrix()));
	//XMStoreFloat4x4(&swordMatrix, XMMatrixTranspose(XMMatrixMultiply(swordLocalMatrix, camParentMatrix))); // TODO: take out translation for both of them and set to zero, multiply, and do simple add for translation and set the matrix 
	// Set the resulting matrix and calc it
	//entity->SetWorldMatrix(swordMatrix);
	// entity->CalcWorldMatrix();

	// TODO: Calculate pos manually using the view matrix 

	// Debugging to check if positions are proper
	//cout << "Camera Position: " << cam->position.x << " " << cam->position.y << " " << cam->position.z << endl;
	//cout << "Sword Position: " << swordMatrix._14 << " " << swordMatrix._24 << " " << swordMatrix._34 << endl;
}

void BloodSword::StartSlash()
{
	ss = SwordState::Raised; 
}

void BloodSword::IdleState()
{
	// not doing any lerping in the idle state
	finalLerpPos = XMFLOAT3(3, -3, 3); // starting pos
}

void BloodSword::RaisedState()
{
	lerpPositionTo = XMFLOAT3(0, 2, 0);

	CalcLerp();

	cout << "Raising" << endl;

	if (CheckTransformationsNearEqual())
	{
		ss = SwordState::Slashing;
	}
}

void BloodSword::SlashingState()
{

	lerpPositionTo = XMFLOAT3(5, 0, 5);
	
	CalcLerp();

	cout << "Slashing" << endl;

	if (CheckTransformationsNearEqual())
	{
		ss = SwordState::Reset;
	}
}

void BloodSword::ResetState()
{
	lerpPositionTo = XMFLOAT3(3, -3, 3);

	CalcLerp();

	cout << "Resetting" << endl;

	if (CheckTransformationsNearEqual())
	{
		ss = SwordState::Idle;
	}
}

bool BloodSword::CheckTransformationsNearEqual()
{
	XMVECTOR lerpPosFrom = XMLoadFloat3(&lerpPositionFrom);
	XMVECTOR lerpPosTo = XMLoadFloat3(&lerpPositionTo);
	XMVECTOR posTolerance = XMLoadFloat3(&positionLerpTolerance);

	return XMVector3NearEqual(lerpPosFrom, lerpPosTo, posTolerance);
}

void BloodSword::CalcLerp() 
{
	XMStoreFloat3(&finalLerpPos, XMVectorLerp(XMLoadFloat3(&lerpPositionFrom), XMLoadFloat3(&lerpPositionTo), deltaTime * positionLerpScalar));

	// finalLerpPos = XMFLOAT3(Utility::FloatLerp(lerpPositionFrom.x, lerpPositionTo.x, deltaTime * positionLerpScalar), Utility::FloatLerp(lerpPositionFrom.y, lerpPositionTo.y, deltaTime * positionLerpScalar), Utility::FloatLerp(lerpPositionFrom.z, lerpPositionTo.z, deltaTime * positionLerpScalar));
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
