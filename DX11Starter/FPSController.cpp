#include "pch.h"
#include "FPSController.h"
#include <WinUser.h>

void FPSController::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
	cam = ScriptManager::EERenderer->GetCamera("main");
	direction = cam->direction; 


	// TODO: Easier setting of physics characteristics via Bullet (coll shape, mass, restitution, other properties)
	
	playerRBody = entity->GetRBody(); // Get the bullet rigidbody
	playerRBody->setAngularFactor(btVector3(0, 1, 0)); // constrain rotations on x and z axes
	// playerRBody->setRestitution(0.1f);
	// playerRBody->setFriction(1.0f); --> debating using friction because it would probably result in weird interations with other level gemoetry, would rather just use pure velocity dampening
	controllerVelocity = btVector3(0, 0, 0);

	collider = entity->GetCollider();

	ps = PlayerState::Normal;
}

void FPSController::Update()
{
	// cout << deltaTime << endl;

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
	XMFLOAT3 zAxis = Z_AXIS;
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction);
	XMVECTOR rightVec = XMVector3Cross(dir, XMLoadFloat3(&yAxis));
	XMFLOAT3 right;
	XMStoreFloat3(&right, rightVec);
	XMVECTOR forwardVec = XMVector3Cross(dir, XMLoadFloat3(&zAxis));
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, forwardVec);

	// update the controller velocity vector based on input

	// base movement
	if (DXCore::keyboard.KeyIsPressed(0x57)) { // w
		controllerVelocity += btVector3(direction.x, 0, direction.z) * spd;
	}
	if (DXCore::keyboard.KeyIsPressed(0x53)) { // s
		controllerVelocity += btVector3(direction.x, 0, direction.z) * -spd;
	}
	if (DXCore::keyboard.KeyIsPressed(0x41)) { // a
		controllerVelocity += btVector3(right.x, 0, right.z) * spd;
	}
	if (DXCore::keyboard.KeyIsPressed(0x44)) { // d
		controllerVelocity += btVector3(right.x, 0, right.z) * -spd;
	}

	// jump/double jump
	if (DXCore::keyboard.OnKeyDown(VK_SPACE)) { 
		if (!midAir || midAir && jumpCount < 2) {
			jumpCount++;
			midAir = true;
			spacingTimer = 0.1f;
			playerRBody->activate();
			playerRBody->applyCentralImpulse(btVector3(0, 35.0f, 0));
			playerRBody->setGravity(btVector3(0.0f, -25.0f, 0.0f));
		}
	}

	// dash
	if (dashCount > 0 && DXCore::keyboard.OnKeyDown(VK_SHIFT))
	{
		dashCount--;
		cout << dashCount << endl;
		// default dash to forwards
		btVector3 dashDirection = btVector3(direction.x, 0, direction.z);

		if (DXCore::keyboard.KeyIsPressed(0x41)) // left
		{
			dashDirection = btVector3(right.x, 0, right.z);
		}
		if (DXCore::keyboard.KeyIsPressed(0x44)) // right
		{
			dashDirection = btVector3(right.x, 0, right.z) * -1;
		}
		if (DXCore::keyboard.KeyIsPressed(0x53)) // backwards
		{
			dashDirection = btVector3(direction.x, 0, direction.z) * -1;
		}

		dashDirection = dashDirection.normalized() * dashScalar;
		
		controllerVelocity += dashDirection;
		// playerRBody->activate();
		// playerRBody->applyCentralImpulse(dashDirection);
	}
	
	// Setting the gradual increase in speed and cappig it
	btScalar ySpd = playerRBody->getLinearVelocity().getY();
	btScalar spd = controllerVelocity.length();
	if (spd > maxSpeed) controllerVelocity = controllerVelocity.normalized() * maxSpeed;
	controllerVelocity.setValue(controllerVelocity.getX(), ySpd, controllerVelocity.getZ());

	DampControllerVelocity();

	playerRBody->activate();
	playerRBody->setLinearVelocity(controllerVelocity);
	// cout << "Vel: (" << controllerVelocity.getX() << ", " << controllerVelocity.getY() << ", " << controllerVelocity.getZ() << ")" << endl;

	XMFLOAT3 eulers = entity->GetEulerAngles();
	eulers = XMFLOAT3(0.0f, eulers.y, 0.0f);
	entity->SetRotation(eulers);

	// cout << deltaTime << endl;
	// cout << "Controller Velocity: (" << controllerVelocity.getX() << ", " << controllerVelocity.getY() << ", " << controllerVelocity.getZ() << ")" << endl;
}

void FPSController::DampControllerVelocity()
{
	btVector3 vel = btVector3(controllerVelocity.getX(), 0, controllerVelocity.getZ());
	if (!vel.fuzzyZero()) 
	{
		controllerVelocity -= vel * dampingScalar; // manual damping for X and Z
	}
	else
	{
		controllerVelocity -= vel; // so we don't get any leftover velocities from fuzzy zero check, zero out the x and z
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

void FPSController::OnCollision(btCollisionObject* other)
{
	Entity* otherE = (Entity*)other->getUserPointer();
	
	// Ground check -- constrain the y velocity if grounded for better control over dynamic controller
	spacingTimer -= deltaTime; // needed to give the jump a chance to startup without getting zeroed out by Floor collision
	if (otherE->tag->c_str() == std::string("Floor") && spacingTimer <= 0.0f)
	{
		midAir = false;
		jumpCount = 0;
		btVector3 vel = playerRBody->getLinearVelocity();
		vel.setValue(vel.getX(), 0.0f, vel.getZ());
		playerRBody->setLinearVelocity(vel);
		playerRBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	}
}

