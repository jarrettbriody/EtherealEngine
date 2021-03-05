#include "pch.h"
#include "FPSController.h"

void FPSController::Init()
{
	cam = ScriptManager::EERenderer->GetCamera("main");
	direction = cam->direction; 

	// TODO: Easier setting of physics characteristics via Bullet (coll shape, mass, restitution, other properties)
	
	playerRBody = entity->GetRBody(); // Get the bullet rigidbody
	playerRBody->setAngularFactor(btVector3(0, 1, 0)); // constrain rotations on x and z axes
	playerRBody->setRestitution(0.1f);
	// playerRBody->setFriction(1.0f); --> debating using friction because it would probably result in weird interations with other level gemoetry, would rather just use pure velocity dampening
	controllerVelocity = btVector3(0, 0, 0);

	collider = entity->GetCollider();

	ps = PlayerState::Normal;
}

void FPSController::Update()
{
	// TODO: Need to get access to delta time

	// player state machine
	switch (ps)
	{
		case PlayerState::Intro:

			break;

		case PlayerState::Normal:
			Move();
			cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y + entity->GetScale().y, entity->GetPosition().z)); // after all updates make sure camera is following the affected entity
			break;

		case PlayerState::HookshotThrown:

			break;

		case PlayerState::HookshotFlight:

			break;

		case PlayerState::HookshotLeash:

			break;

		case PlayerState::Paused:

			break;

		case PlayerState::Death:
			// ragdoll the player
			playerRBody->setAngularFactor(btVector3(1, 1, 1)); // constrain rotations on x and z axes
			playerRBody->setGravity(btVector3(0.0f, -25.0f, 0.0f));
			cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y + entity->GetScale().y, entity->GetPosition().z)); // after all updates make sure camera is following the affected entity

			break;

		case PlayerState::Victory:
			
			break;

		default:

			break;
	}
}

void FPSController::Move()
{
	// Choosing to go with a dynamic character controller to ensure ease of interaction with Bullet, many people online mentioned funky things going on when trying to get kinematic to work

	// ready the needed information
	direction = cam->direction;
	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction);
	XMVECTOR rightVec = XMVector3Cross(dir, XMLoadFloat3(&yAxis));
	XMFLOAT3 right;
	XMStoreFloat3(&right, rightVec);

	GroundCheck();

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
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) { // TODO: Need to detect input for a single keypress down for double jump (probably other things too)
		if (!midAir) {
			cout << "Jump" << endl;
			playerRBody->applyCentralImpulse(btVector3(0, 30.0f, 0));
			midAir = true;
			jumpCount++;
		}
	}
	/*
	TODO: Dash
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) {
		
	}
	*/
	
	btScalar ySpd = playerRBody->getLinearVelocity().getY();
	btScalar spd = controllerVelocity.length();
	if (spd > maxSpeed) controllerVelocity = controllerVelocity.normalized() * maxSpeed;
	controllerVelocity.setValue(controllerVelocity.getX(), ySpd, controllerVelocity.getZ());

	DampControllerVelocity();
	playerRBody->setLinearVelocity(controllerVelocity);
	// cout << "Vel: (" << controllerVelocity.getX() << ", " << controllerVelocity.getY() << ", " << controllerVelocity.getZ() << ")" << endl;

	XMFLOAT3 eulers = entity->GetEulerAngles();
	eulers = XMFLOAT3(0.0f, eulers.y, 0.0f);
	entity->SetRotation(eulers);
}

void FPSController::DampControllerVelocity()
{
	btVector3 vel = btVector3(controllerVelocity.getX(), 0, controllerVelocity.getZ());
	if (!vel.fuzzyZero()) 
	{
		vel *= dampingScalar;
		controllerVelocity -= vel; // manual damping for X and Z
		// cout << "DAMPING" << endl;
	}
	else
	{
		controllerVelocity -= vel; // so we don't get any leftover velocities from fuzzy zero check, zero out the x and z
	}
}

void FPSController::GroundCheck()
{
	// TODO: Properly check for the ground using Bullet
	// Ground Check
	if (entity->CheckSATCollision((*eMap)["Floor (8)"])) {
		midAir = false;
		jumpCount = 0;
		btVector3 vel = playerRBody->getLinearVelocity();
		vel.setValue(vel.getX(), 0.0f, vel.getZ());
		playerRBody->setLinearVelocity(vel);
	}

	// Only apply gravity to the player when in the air
	if (!midAir)
	{
		playerRBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	}
	else
	{
		playerRBody->setGravity(btVector3(0.0f, -25.0f, 0.0f));
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

