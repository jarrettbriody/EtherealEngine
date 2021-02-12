#include "FPSController.h"

void FPSController::Init()
{
	cam = ScriptManager::renderer->GetCamera("main");
	direction = cam->direction; 

	// TODO: Easier setting of physics characteristics via Bullet (coll shape, mass, restitution, other properties)
	
	playerRBody = entity->GetRBody(); // Get the bullet rigidbody
}

void FPSController::Update()
{
	// TODO: Figure out how much of the following is still needed
	direction = cam->direction;
	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction); 
	dir.m128_f32[1] = 0.0f;
	XMVECTOR right = XMVector3Cross(dir, XMLoadFloat3(&yAxis));

	Move();

	// update camera pos from player physics transform
	btVector3 childCameraTransformPos = eMap->find("Camera")->second->GetRBody()->getCenterOfMassPosition(); //find the camera which is the child entity of fps controller  // playerRBody->getCenterOfMassPosition();
	cout << childCameraTransformPos.getY() << endl;
	cam->SetPosition(XMFLOAT3(childCameraTransformPos.getX(), childCameraTransformPos.getY(), childCameraTransformPos.getZ()));
}

void FPSController::Move()
{

	btVector3 linearVelocity;
	btVector3 moveVec;

	if (GetAsyncKeyState('W') & 0x8000) {
		linearVelocity = btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f);
		moveVec = btVector3(0.0f, 0.0f, 100.0f);
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		linearVelocity = btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f);
		moveVec = btVector3(0.0f, 0.0f, -100.0f);
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		linearVelocity = btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f);
		moveVec = btVector3(-100.0f, 0.0f, 0.0f);
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		linearVelocity = btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f);
		moveVec = btVector3(100.0f, 0.0f, 0.0f);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		linearVelocity = btVector3(playerRBody->getLinearVelocity().getX(), 0.0f, playerRBody->getLinearVelocity().getZ());
		moveVec = btVector3(0.0f, 15.0f, 0.0f);
	}

	playerRBody->activate();
	playerRBody->setLinearVelocity(linearVelocity);
	playerRBody->applyCentralForce(moveVec);
}

void FPSController::OnMouseMove(WPARAM buttonState, int x, int y)
{
	if (buttonState & 0x0001) {
		cam->RotateCamera(x - (int)prevMousePos.x, y - (int)prevMousePos.y);

		prevMousePos.x = x;
		prevMousePos.y = y;
	}
}

/*
void SceneManager::CheckCollisionWithFloor()
{
	int numManifolds = dispatcher->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		auto* obA = contactManifold->getBody0();
		auto* obB = contactManifold->getBody1();

		if (obA->getUserPointer() == "Player" && obB->getUserPointer() == "Floor")
		{
			int numContacts = contactManifold->getNumContacts();
			if (numContacts > 0 && !doubleJumpControl)
			{
				jumpCount = 0;
				doubleJumpControl = false;
			}
		}
	}
}
*/
