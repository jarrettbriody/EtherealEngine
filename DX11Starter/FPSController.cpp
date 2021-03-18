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
	impulseSumVec = btVector3(0, 0, 0);

	if (midAir)
	{
		playerRBody->setGravity(btVector3(0.0f, -25.0f, 0.0f)); // set gravity again when in the air
	}
	else
	{
		playerRBody->setGravity(btVector3(0.0f, 0.0f, 0.0f)); // set gravity again when on the ground
	}

	collider = entity->GetCollider();

	ps = PlayerState::Normal;
}

void FPSController::Update()
{
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
			playerRBody->setAngularFactor(btVector3(1, 1, 1)); // free rotations on x and z axes
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

	// Set gravity 
	if (midAir)
	{
		playerRBody->setGravity(btVector3(0.0f, -25.0f, 0.0f));
	}
	else
	{
		playerRBody->setGravity(btVector3(0.0f, 0.0f, 0.0f)); // TODO: Current issue with this is that if you walk off a heightened platform without jumping you won't fall, may have to do raycast check for distance to ground
	}

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
	if (spacingTimer > 0)
	{
		spacingTimer -= deltaTime; // needed to give the jump a chance to startup without getting zeroed out by Floor collision
	}
	
	btVector3 jumpForce = btVector3(0, 0, 0);
	if (DXCore::keyboard.OnKeyDown(VK_SPACE)) { 
		if (!midAir || midAir && jumpCount < 2) {
			
			if (!midAir)
			{
				jumpForce = btVector3(direction.x, 10.0f, direction.z) * jumpForceScalar;
			}
			else
			{
				jumpForce = btVector3(direction.x, 12.0f * jumpForceScalar, direction.z); // not allowing as much lateral movement on second jump but giving more height
			}
			
			jumpCount++;
			midAir = true;
			spacingTimer = 0.1f;
		}
	}
	
	// dash
	if (dashDampTimer > 0)
	{
		dashDampTimer -= deltaTime;
	}

	btVector3 dashImpulse = btVector3(0, 0, 0);
	if (/*dashCount > 0 &&*/ DXCore::keyboard.OnKeyDown(VK_SHIFT))
	{
		dashCount--;
		cout << dashCount << endl;
		// default dash to forwards
		dashImpulse = btVector3(direction.x, 0, direction.z) * dashImpulseScalar;

		if (DXCore::keyboard.KeyIsPressed(0x41)) // left
		{
			dashImpulse = btVector3(right.x, 0, right.z) * dashImpulseScalar;
		}
		if (DXCore::keyboard.KeyIsPressed(0x44)) // right
		{
			dashImpulse = btVector3(right.x, 0, right.z) * -dashImpulseScalar;
		}
		if (DXCore::keyboard.KeyIsPressed(0x53)) // backwards
		{
			dashImpulse = btVector3(direction.x, 0, direction.z) * -dashImpulseScalar;
		}

		dashDampTimer = 0.25f;
	}
	
	// Ensuring current speed does not overtake maxSpeed
	btScalar ySpd = playerRBody->getLinearVelocity().getY();
	btScalar spd = controllerVelocity.length();
	if (spd > maxSpeed) controllerVelocity = controllerVelocity.normalized() * maxSpeed;
	controllerVelocity.setValue(controllerVelocity.getX(), ySpd, controllerVelocity.getZ());
	
	// Sum of impulse forces (for now just dash)
	impulseSumVec += dashImpulse;

	// Damping
	DampForces();

	// FORCES ADDED TO RIGIDBODY 
	playerRBody->activate();
	playerRBody->setLinearVelocity(controllerVelocity + jumpForce);
	playerRBody->applyCentralImpulse(impulseSumVec);
	// cout << "Vel: (" << controllerVelocity.getX() << ", " << controllerVelocity.getY() << ", " << controllerVelocity.getZ() << ")" << endl;

	// set Ethereal Engine rotations
	XMFLOAT3 eulers = entity->GetEulerAngles();
	eulers = XMFLOAT3(0.0f, eulers.y, 0.0f);
	entity->SetRotation(eulers);

	// cout << deltaTime << endl;
	// cout << "Controller Velocity: (" << controllerVelocity.getX() << ", " << controllerVelocity.getY() << ", " << controllerVelocity.getZ() << ")" << endl;
}

void FPSController::DampForces()
{
	if (dashDampTimer <= 0) // always damp the impulse vec unless player is the player just initiated a dash
	{
		impulseSumVec -= impulseSumVec * dampingScalar;
	}

	if (DXCore::keyboard.NoKeyDown() && !midAir) // Only damp overall movement if nothing is being pressed while on the ground
	{
		controllerVelocity -= controllerVelocity * dampingScalar;
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
	if (otherE->tag->c_str() == std::string("Floor") && spacingTimer <= 0.0f)
	{
		midAir = false;
		jumpCount = 0;
	}
}

