#include "pch.h"
#include "FPSController.h"

void FPSController::Init()
{
	cam = ScriptManager::EERenderer->GetCamera("main");
	direction = cam->direction; 

	// TODO: Easier setting of physics characteristics via Bullet (coll shape, mass, restitution, other properties)
	
	playerRBody = entity->GetRBody(); // Get the bullet rigidbody
	playerRBody->setAngularFactor(btVector3(0, 1, 0)); // constrain rotations on x and z axes
	playerRBody->setGravity(btVector3(0.0f, -25.0f, 0.0f));
	playerRBody->setRestitution(0.1f);
	// playerRBody->setFriction(1.0f); --> debating using friction because it would probably result in weird interations with other level gemoetry, would rather just use pure velocity dampening
	controllerVelocity = btVector3(0, 0, 0);

	collider = entity->GetCollider();
}

void FPSController::Update()
{
	Move();
	cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y + entity->GetScale().y, entity->GetPosition().z));
}

void FPSController::Move()
{
	// Choosing to go with a dynamic character controller to ensure ease of interaction with Bullet, many people online mentioned funk things going on when trying to get kinematic to work

	// ready the needed information
	direction = cam->direction;
	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction);
	XMVECTOR rightVec = XMVector3Cross(dir, XMLoadFloat3(&yAxis));
	XMFLOAT3 right;
	XMStoreFloat3(&right, rightVec);

	// TODO: Properly enforce damping when moving on the ground
	if(!midAir)
		playerRBody->setDamping(0.99f, 0.99f);

	// TODO: Properly check for the ground
	// Ground Check
	if (entity->CheckSATCollision((*eMap)["Floor"])) {
		midAir = false;
		jumpCount = 0;
		btVector3 vel = playerRBody->getLinearVelocity();
		vel.setValue(vel.getX(), 0.0f, vel.getZ());
		playerRBody->setLinearVelocity(vel);
	}

	// update the controller velocity vector based on input
	playerRBody->activate();

	if (GetAsyncKeyState('W') & 0x8000) {
		controllerVelocity += btVector3(direction.x, 0, direction.z) * spd;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		controllerVelocity += btVector3(direction.x, 0, direction.z) * -spd;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		controllerVelocity += btVector3(right.x, 0, right.z) * spd;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		controllerVelocity += btVector3(right.x, 0, right.z) * -spd;
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		if (!midAir || (midAir && jumpCount < 2)) {
			playerRBody->applyCentralImpulse(btVector3(0, 20.0f, 0));
			midAir = true;
			jumpCount++;
		}
	}
	
	/* TODO: Dash
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
	
	}
	*/
	
	btScalar ySpd = playerRBody->getLinearVelocity().getY();
	btScalar spd = controllerVelocity.length();
	if (spd > maxSpeed) controllerVelocity = controllerVelocity.normalized() * maxSpeed;
	controllerVelocity.setValue(controllerVelocity.getX(), ySpd, controllerVelocity.getZ());
	
	playerRBody->setLinearVelocity(controllerVelocity);

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

//void FPSController::CheckCollisionWithFloor()
//{
//	int numManifolds = dispatcher->getNumManifolds();
//	for (int i = 0; i < numManifolds; i++)
//	{
//		btPersistentManifold* contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
//		auto* obA = contactManifold->getBody0();
//		auto* obB = contactManifold->getBody1();
//
//		if (obA->getUserPointer() == "Player" && obB->getUserPointer() == "Floor")
//		{
//			int numContacts = contactManifold->getNumContacts();
//			if (numContacts > 0 && !doubleJumpControl)
//			{
//				jumpCount = 0;
//				doubleJumpControl = false;
//			}
//		}
//	}
//}

