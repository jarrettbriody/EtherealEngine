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
	Move();

	cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y, entity->GetPosition().z));
}

void FPSController::Move()
{
	// ready the needed information
	direction = cam->direction;
	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction);
	dir.m128_f32[1] = 0.0f;
	XMVECTOR right = XMVector3Cross(dir, XMLoadFloat3(&yAxis));

	// TODO: Input detection that is only for key press down
	// apply forces based on input
	playerRBody->activate();
	if (GetAsyncKeyState('W') & 0x8000) {
		playerRBody->setLinearVelocity(btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f));
		playerRBody->applyCentralImpulse(btVector3(direction.x, 0, direction.z) * 25.0f);
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		playerRBody->setLinearVelocity(btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f));
		playerRBody->applyCentralImpulse(btVector3(direction.x, 0, direction.z) * -25.0f);
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		playerRBody->setLinearVelocity(btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f));
		playerRBody->applyCentralImpulse(btVector3(right.m128_f32[0], 0, right.m128_f32[2]) * 25.0f);
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		playerRBody->setLinearVelocity(btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f));
		playerRBody->applyCentralImpulse(btVector3(right.m128_f32[0], 0, right.m128_f32[2]) * -25.0f);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		playerRBody->setLinearVelocity(btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f));
		playerRBody->applyCentralImpulse(btVector3(0.0f, 2.5f, 0.0f));
	}
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
