#include "pch.h"
#include "FPSController.h"

void FPSController::Init()
{
	cam = ScriptManager::EERenderer->GetCamera("main");
	direction = cam->direction; 

	// TODO: Easier setting of physics characteristics via Bullet (coll shape, mass, restitution, other properties)
	
	playerRBody = entity->GetRBody(); // Get the bullet rigidbody
	playerRBody->setAngularFactor(btVector3(0, 1, 0)); // constrain rotations on x and z axes
	collider = entity->GetCollider();
}

void FPSController::Update()
{
	Move();
	cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y + entity->GetScale().y, entity->GetPosition().z));
}

void FPSController::Move()
{
	// ready the needed information
	direction = cam->direction;
	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction);
	XMVECTOR rightVec = XMVector3Cross(dir, XMLoadFloat3(&yAxis));
	XMFLOAT3 right;
	XMStoreFloat3(&right, rightVec);

	if(!midAir)
		playerRBody->setDamping(0.95f, 0.0f);

	if (entity->CheckSATCollision((*eMap)["Floor"])) {
		midAir = false;
		jumpCount = 0;
		btVector3 vel = playerRBody->getLinearVelocity();
		vel.setValue(vel.getX(), 0.0f, vel.getZ());
		playerRBody->setLinearVelocity(vel);
	}

	// TODO: Input detection that is only for key press down
	// apply forces based on input
	playerRBody->activate();
	if (GetAsyncKeyState('W') & 0x8000) {
		//playerRBody->setLinearVelocity(btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f));
		playerRBody->applyCentralImpulse(btVector3(direction.x, 0, direction.z) * spd);
		playerRBody->setDamping(0.0f, 0.0f);
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		//playerRBody->setLinearVelocity(btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f));
		playerRBody->applyCentralImpulse(btVector3(direction.x, 0, direction.z) * -spd);
		playerRBody->setDamping(0.0f, 0.0f);
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		//playerRBody->setLinearVelocity(btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f));
		playerRBody->applyCentralImpulse(btVector3(right.x, 0, right.z) * spd);
		playerRBody->setDamping(0.0f, 0.0f);
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		//playerRBody->setLinearVelocity(btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f));
		playerRBody->applyCentralImpulse(btVector3(right.x, 0, right.z) * -spd);
		playerRBody->setDamping(0.0f, 0.0f);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		if (!midAir || (midAir && jumpCount < 2)) {
			//playerRBody->setLinearVelocity(btVector3(0.0f, playerRBody->getLinearVelocity().getY(), 0.0f));
			playerRBody->applyCentralImpulse(btVector3(0.0f, 20.0f, 0.0f));
			midAir = true;
			jumpCount++;
		}
	}

	btVector3 vel = playerRBody->getLinearVelocity();
	btScalar ySpd = vel.getY();
	vel.setValue(vel.getX(), 0.0f, vel.getZ());
	btScalar spd = vel.length();
	if (spd > maxSpeed) vel = vel.normalized() * maxSpeed;
	vel.setValue(vel.getX(), ySpd, vel.getZ());
	playerRBody->setLinearVelocity(vel);

	XMFLOAT3 eulers = entity->GetEulerAngles();
	eulers = XMFLOAT3(0.0f, eulers.y, 0.0f);
	entity->SetRotation(eulers);
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
