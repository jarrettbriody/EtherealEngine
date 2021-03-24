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
			CheckAbilities();
			Move();
			cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y + entity->GetScale().y + headbobOffset, entity->GetPosition().z)); // after all updates make sure camera is following the affected entity
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

			cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y + entity->GetScale().y + headbobOffset, entity->GetPosition().z)); // after all updates make sure camera is following the affected entity
			break;

		case PlayerState::Victory:
			
			break;

		default:

			break;
	}
}


void FPSController::CheckAbilities()
{
	Hookshot();
}

void FPSController::Hookshot()
{
	if (DXCore::keyboard.OnKeyDown(0x45)) // e
	{
		Config::DynamicsWorld->updateAabbs();
		Config::DynamicsWorld->computeOverlappingPairs();

		// Redefine our vectors using bullet's silly types
		btVector3 from(entity->GetPosition().x, entity->GetPosition().y, entity->GetPosition().z);
		btVector3 to(entity->GetPosition().x + direction.x, entity->GetPosition().y + direction.y, entity->GetPosition().z + direction.z); // raycast direction the camera is looking

		// debug
		DebugLines* hookshotDebugLines = new DebugLines("hookshotDebugLines", 0, false); // cannot turn on the willUpdate paramater currently because not sure how to figure out which lines to update via the input Bullet gives 
		XMFLOAT4X4 wm;
		XMStoreFloat4x4(&wm, XMMatrixTranspose(DirectX::XMMatrixIdentity()));
		hookshotDebugLines->worldMatrix = wm;
		hookshotDebugLines->color = XMFLOAT3(0.0f, 0.0f, 1.0f);

		XMFLOAT3 fromVec = XMFLOAT3(from.getX(), from.getY(), from.getZ());
		XMFLOAT3 toVec = XMFLOAT3(to.getX(), to.getY(), to.getZ());
		XMFLOAT3* linePoints = new XMFLOAT3[8];
		linePoints[0] = fromVec;
		linePoints[1] = fromVec;
		linePoints[2] = fromVec;
		linePoints[3] = fromVec;
		linePoints[4] = toVec;
		linePoints[5] = toVec;
		linePoints[6] = toVec;
		linePoints[7] = toVec;
		hookshotDebugLines->GenerateCuboidVertexBuffer(linePoints, 8);
		delete[] linePoints;

		// Create variable to store the ray hit and set flags
		btCollisionWorld::ClosestRayResultCallback closestResult(from, to);
		closestResult.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

		Config::DynamicsWorld->rayTest(from, to, closestResult); // Raycast

		if (closestResult.hasHit()) // if there is a surface to stand on
		{
			// Get the entity associated with the rigid body we hit
			Entity* hit = (Entity*)(closestResult.m_collisionObject->getUserPointer());
			printf("Hookshot Hit: %s\n", hit->GetName().c_str());

			// impulseSumVec += (closestResult.m_hitPointWorld - from).normalize() * 10.0f;
		}
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
	XMStoreFloat3(&right, rightVec);

	// update the controller velocity vector based on input
	GroundCheck();
	UpdateHeadbob();
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
	btVector3 jumpForce = JumpForceFromInput();
	
	// dash
	btVector3 dashImpulse = DashImpulseFromInput();
	
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

void FPSController::GroundCheck()
{
	// Ground check
	Config::DynamicsWorld->updateAabbs();
	Config::DynamicsWorld->computeOverlappingPairs();

	// Redefine our vectors using bullet's silly types
	btVector3 from(entity->GetPosition().x, entity->GetPosition().y, entity->GetPosition().z);
	btVector3 to(entity->GetPosition().x, entity->GetPosition().y - 1, entity->GetPosition().z); // check a little below the player for any surface to stand on 

	// Create variable to store the ray hit and set flags
	btCollisionWorld::ClosestRayResultCallback closestResult(from, to);
	closestResult.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

	Config::DynamicsWorld->rayTest(from, to, closestResult); // Raycast

	if (closestResult.hasHit()) // if there is a surface to stand on
	{
		// Get the entity associated with the rigid body we hit
		/*Entity* hit = (Entity*)(closestResult.m_collisionObject->getUserPointer());
		printf("Hit: %s\n", hit->GetName().c_str());*/
		
		midAir = false;
		playerRBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		jumpCount = 0;
	}
	else
	{
		midAir = true;
		playerRBody->setGravity(btVector3(0.0f, -25.0f, 0.0f));
	}
}

void FPSController::UpdateHeadbob()
{
	// TODO: Right now framrerate will cause this to move faster

	if (!DXCore::keyboard.NoKeyDown() && !midAir) // if keys are down and we are on the ground we want to headbob
	{
		if (headbobOffset < HEADBOB_OFFSET_MAX && !resetHeadbob) // increase headbob offset if it is less than the max and we are not resetting 
		{
			headbobOffset += HEADBOB_OFFSET_INTERVAL;
		}
		else
		{
			resetHeadbob = true; // reset headbob sice we reached the max
		}

		if (headbobOffset > HEADBOB_OFFSET_MIN && resetHeadbob) // decrease headbob offset if it is greater than the min and we are resetting
		{
			headbobOffset -= HEADBOB_OFFSET_INTERVAL;
		}
		else
		{
			resetHeadbob = false; // resetting is complete after reaching the min
		}

	}
	else // return to min headbob position if no keys are down or we are in midair
	{
		if (headbobOffset > HEADBOB_OFFSET_MIN)
		{
			headbobOffset -= HEADBOB_OFFSET_INTERVAL;
		}
	}

	// cout << headbobOffset << endl;
}

btVector3 FPSController::JumpForceFromInput()
{
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
		}
	}

	return jumpForce;
}

btVector3 FPSController::DashImpulseFromInput()
{
	if (dashDampTimer > 0)
	{
		dashDampTimer -= deltaTime;
	}

	btVector3 dashImpulse = btVector3(0, 0, 0);
	if (/*dashCount > 0 &&*/ DXCore::keyboard.OnKeyDown(VK_SHIFT))
	{
		dashCount--;
		// cout << dashCount << endl;
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

	return dashImpulse;
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
	if (buttonState & 0x0001) { // holding LMB to look around only happens if Config::FPSControllerEnabled = false
		cam->RotateCamera(x - (int)prevMousePos.x, y - (int)prevMousePos.y); 

		prevMousePos.x = x;
		prevMousePos.y = y;
	}
}

void FPSController::OnMouseDown(WPARAM buttonState, int x, int y)
{
	if (buttonState & 0x0001) {
		// sword slash
		cout << "LMB" << endl;
	}
}

void FPSController::OnCollision(btCollisionObject* other)
{
	Entity* otherE = (Entity*)other->getUserPointer();
	
}



