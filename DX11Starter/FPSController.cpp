#include "pch.h"
#include "FPSController.h"


void FPSController::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
	cam = ScriptManager::EERenderer->GetCamera("main");
	prevMousePos.x = 0;
	prevMousePos.y = 0;
	direction = cam->direction; 
	cam->SetFOV(fov);

	icicleParams = {
			"Blood Icicle",					// name
			"Blood Icicle",					// tag
			"Blood Icicle",					// layer
			"Cone",							// mesh
			"Red",							// material
			{"BLOODICICLE"},				// script names
			1,								// script count
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// position
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// rotation
			XMFLOAT3(0.5f, 8.0f, 0.5f),		// scale
			1.0f,							// mass
			true,
			BulletColliderShape::CAPSULE
			// defaults work for the rest
	};

	/*
	swordParams = {
			"Blood Sword",					// name
			"Blood Sword",					// tag
			"Blood Sword",					// layer
			"bloodsword",							// mesh
			"Red",							// material
			{"BLOODSWORD"},				// script names
			1,								// script count
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// position
			XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f),		// rotation
			XMFLOAT3(1.0f, 1.0f, 1.0f),		// scale
			0.0f,							// mass
			false
			// defaults work for the rest
	};

	sword = ScriptManager::CreateEntity(swordParams);
	sword->collisionsEnabled = false;
	*/
	sword = (*eMap)["Blood Sword"];
	
	bloodOrb = eMap->find("Blood_Orb")->second;

	hookshotParams = {
			"Hookshot",					// name
			"Hookshot",					// tag
			"Hookshot",					// layer
			"bloodchain",							// mesh
			"bloodchain",							// material
			{""},				// script names
			0,								// script count
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// position
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// rotation
			XMFLOAT3(1.0f, 1.0f, hookshotZScale),		// scale
			0.0f,							// mass
			false								// defaults work for the rest
	};

	hookshot = ScriptManager::CreateEntity(hookshotParams);
	//hookshot->collisionsEnabled = false;

	playerRBody = entity->GetRBody(); // Get the bullet rigidbody
	playerRBody->setAngularFactor(btVector3(0, 1, 0)); // constrain rotations on x and z axes
	// playerRBody->setRestitution(0.1f);
	// playerRBody->setFriction(1.0f); --> debating using friction because it would probably result in weird interations with other level gemoetry, would rather just use pure velocity dampening

	keyboard = Keyboard::GetInstance();
	mouse = Mouse::GetInstance();

	controllerVelocity = btVector3(0, 0, 0);
	impulseSumVec = btVector3(0, 0, 0);
	hookshotPoint = btVector3(0, 0, 0);

	collider = entity->GetCollider();

	ps = PlayerState::Normal;

	dashBlurCallback.vShader = EESceneLoader->vertexShadersMap["PostProcess"];
	dashBlurCallback.pShader = EESceneLoader->pixelShadersMap["DashBlur"];
	EERenderer->SetPostProcess(true, &dashBlurCallback, 1);
}

void FPSController::Update()
{
	// player state machine
	switch (ps)
	{
		/*
		* A - Move FPS controller
		* B - Rotate Camera
		* C - Everything else
		*/
		case PlayerState::Intro:

			break;

		case PlayerState::Normal:
			Move();
			MouseLook();
			cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y + entity->GetScale().y + headbobOffset, entity->GetPosition().z)); // after all updates make sure camera is following the affected entity
			CheckAllAbilities();
			break;
		
		case PlayerState::HookshotThrow:
			HookshotThrow();
			MouseLook();
			cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y + entity->GetScale().y + headbobOffset, entity->GetPosition().z)); // after all updates make sure camera is following the affected entity
			UpdateHookShotTransform();
			CheckBloodSword();
			CheckBloodIcicle();
			CheckBulletTime();
			break;

		case PlayerState::HookshotFlight:
			HookshotFlight();
			MouseLook();
			cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y + entity->GetScale().y + headbobOffset, entity->GetPosition().z)); // after all updates make sure camera is following the affected entity
			UpdateHookShotTransform();
			CheckBloodSword();
			CheckBloodIcicle();
			CheckBulletTime();
			break;

		case PlayerState::HookshotLeash:
			HookshotLeash();
			Move();
			MouseLook();
			cam->SetPosition(XMFLOAT3(entity->GetPosition().x, entity->GetPosition().y + entity->GetScale().y + headbobOffset, entity->GetPosition().z)); // after all updates make sure camera is following the affected entity
			UpdateHookShotTransform();
			CheckBloodSword();
			CheckBloodIcicle();
			CheckBulletTime();
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


void FPSController::CheckAllAbilities()
{
	CheckBloodSword(); 
	CheckBloodIcicle();
	CheckHookshot();
	CheckBulletTime();
}

void FPSController::CheckBloodSword()
{
	if (mouse->OnLMBDown())
	{
		BloodSword* swordScript = (BloodSword*)scriptFunctionsMap[sword->GetName()]["BLOODSWORD"];
		swordScript->StartSlash();
	}
}


void FPSController::CheckBloodIcicle()
{
	if (mouse->OnRMBDown() && bloodIcicleCooldownTimer <= 0) 
	{
		// update position and rotation of the EntityCreationParams
		icicleParams.position = bloodOrb->GetPosition();
		icicleParams.rotationRadians = XMFLOAT3(cam->xRotation + 1.5708f /* 90 degress in radians */ , cam->yRotation, cam->zRotation);

		Entity* bloodIcicle = ScriptManager::CreateEntity(icicleParams);

		btVector3 shotImpulse = Utility::Float3ToBulletVector(cam->direction);

		bloodIcicle->GetRBody()->setGravity(btVector3(0,0,0));
		bloodIcicle->GetRBody()->activate();
		bloodIcicle->GetRBody()->applyCentralImpulse(shotImpulse.normalized() * bloodIcicleScalar);

		// backwards recoil impulse on player
		impulseSumVec += btVector3(-direction.x, 0, -direction.z).normalized() * bloodIcicleRecoilScalar;

		bloodIcicleCooldownTimer = BLOOD_ICICLE_MAX_COOLDOWN_TIME;
	}
	else if(bloodIcicleCooldownTimer > 0)
	{
		bloodIcicleCooldownTimer -= deltaTime;
	}
}

void FPSController::CheckBulletTime()
{
	// Slow time instantly and keep it slowed while Q is pressed but gradually ramp time back up to normal time when not pressed 
	if (keyboard->KeyIsPressed(0x51)) 
	{
		bulletTimeRampDown = BULLET_TIME_SCALAR;
		DXCore::deltaTimeScalar = BULLET_TIME_SCALAR;
	}
	else
	{
		if (bulletTimeRampDown < NORMAL_TIME_SCALAR)
		{
			bulletTimeRampDown += deltaTime;
		}
		else
		{
			bulletTimeRampDown = NORMAL_TIME_SCALAR;
		}

		DXCore::deltaTimeScalar = bulletTimeRampDown;
	}
}

void FPSController::CheckHookshot()
{
	if (keyboard->OnKeyDown(0x45)) // E
	{
		Config::DynamicsWorld->updateAabbs();
		Config::DynamicsWorld->computeOverlappingPairs();

		// Redefine our vectors using bullet's types
		btVector3 from(Utility::Float3ToBulletVector(bloodOrb->GetPosition()));
		btVector3 to(from.getX() + direction.x * hookshotRangeScalar, from.getY() + direction.y * hookshotRangeScalar, from.getZ() + direction.z * hookshotRangeScalar); // raycast direction the camera is looking

		// debug line
		/*
		DebugLines* hookshotDebugLines = new DebugLines("hookshotDebugLines", 0, false); // cannot turn on the willUpdate paramater currently because not sure how to figure out which lines to update via the input Bullet gives 
		XMFLOAT4X4 wm;
		XMStoreFloat4x4(&wm, XMMatrixTranspose(DirectX::XMMatrixIdentity()));
		hookshotDebugLines->worldMatrix = wm;
		hookshotDebugLines->color = XMFLOAT3(0.0f, 0.0f, 1.0f);

		XMFLOAT3 fromVec = Utility::BulletVectorToFloat3(from);
		XMFLOAT3 toVec = Utility::BulletVectorToFloat3(to);
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
		*/
		

		// Create variable to store the ray hit and set flags
		btCollisionWorld::ClosestRayResultCallback closestResult(from, to);
		closestResult.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

		Config::DynamicsWorld->rayTest(from, to, closestResult); // Raycast

		if (closestResult.hasHit()) // if there is a surface to grapple to
		{
			// Get the entity associated with the rigid body we hit
			PhysicsWrapper* wrapper = (PhysicsWrapper*)closestResult.m_collisionObject->getUserPointer();

			hookshotPoint = closestResult.m_hitPointWorld;

			if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY) 
			{
				// Entity* hit = (Entity*)wrapper->objectPointer;
				hookshotAttachedEntity = (Entity*)wrapper->objectPointer;

				ps = PlayerState::HookshotThrow;
			}
		}
	}
}

void FPSController::HookshotThrow()
{
	if (hookshotZScale < hookshotLength)
	{
		hookshotZScale += hookshotThrowSpeed * deltaTime;
	}
	else
	{
		if (hookshotAttachedEntity->tag.STDStr() == std::string("Enemy") || hookshotAttachedEntity->tag.STDStr() == std::string("Body Part"))
		{
			leashedEnemy = hookshotAttachedEntity;
			leashSize = playerRBody->getCenterOfMassPosition().distance(leashedEnemy->GetRBody()->getCenterOfMassPosition()); 
			// cout << leashSize << endl;

			ps = PlayerState::HookshotLeash;
		}
		else if (hookshotAttachedEntity->tag.STDStr() == std::string("Environment"))
		{
			// playerRBody->clearForces(); --> don't know if needed

			ps = PlayerState::HookshotFlight;
		}
		else
		{
			ResetHookshotTransform();
		}
	}
}

void FPSController::HookshotFlight()
{
	playerRBody->setGravity(btVector3(0,0,0));

	if (keyboard->KeyIsPressed(0x45))
	{
		if (hookshotZScale < hookshotLength)
		{
			hookshotZScale += hookshotThrowSpeed * deltaTime;
		}
		else if(hookshotZScale > hookshotLength)
		{
			hookshotZScale -= hookshotThrowSpeed * deltaTime;
		}

		btVector3 playerCenterOfMassPos = playerRBody->getCenterOfMassPosition();

		btScalar distanceToHitPoint = playerCenterOfMassPos.distance(hookshotPoint);

		playerRBody->activate();
		playerRBody->applyCentralForce(controllerVelocity.normalized() + (hookshotPoint - playerCenterOfMassPos).normalized() * distanceToHitPoint * 2.0f); // adjust speed according to distance away with an added small scalar

		if (distanceToHitPoint < EXIT_RANGE)
		{
			ResetHookshotTransform();
		}
	}
	else // cancel if not holding E
	{
		ResetHookshotTransform();
	}
}

void FPSController::HookshotLeash()
{
	if (hookshotZScale < hookshotLength)
	{
		hookshotZScale += hookshotThrowSpeed * deltaTime;
	}
	else if (hookshotZScale > hookshotLength)
	{
		hookshotZScale -= hookshotThrowSpeed * deltaTime;
	}

	if (leashPullCooldownTimer > 0)
	{
		leashPullCooldownTimer -= deltaTime;
	}

	// pull enemy into range if they are "stretching" over the initial leash size
	btVector3 playerCenterOfMassPos = playerRBody->getCenterOfMassPosition();
	float leashDistanceToEnemy = playerCenterOfMassPos.distance(leashedEnemy->GetRBody()->getCenterOfMassPosition()); 
	if (leashDistanceToEnemy > leashSize && leashPullCooldownTimer <= 0)
	{
		leashedEnemy->GetRBody()->activate();
		leashedEnemy->GetRBody()->applyCentralImpulse((playerCenterOfMassPos - leashedEnemy->GetRBody()->getCenterOfMassPosition()).normalized() * leashedScalar * (1/leashedEnemy->GetRBody()->getInvMass())); // scale the force of the impulse in ratio to the mass of the leashed object
		leashPullCooldownTimer = LEASH_PULL_MAX_COOLDOWN_TIME; // we don't want to contiually apply impulses all the time because that makes the leashed enemy go beserk. The timer allows us to manage how often the impulse is applied
	}

	if (keyboard->OnKeyDown(0x45)) // enemy pull cancel after pressing E again
	{
		ResetHookshotTransform();

		// Pull enemy towards player when canceling the leash
		leashedEnemy->GetRBody()->applyCentralImpulse((playerCenterOfMassPos - leashedEnemy->GetRBody()->getCenterOfMassPosition()).normalized() * leashCancelScalar * (1 / leashedEnemy->GetRBody()->getInvMass()));
	}

	if (keyboard->OnKeyDown(VK_SPACE)) // pull player to enemy cancel after press space 
	{
		ResetHookshotTransform();

		// Pull player towards enemy when canceling the leash
		impulseSumVec += (leashedEnemy->GetRBody()->getCenterOfMassPosition() - playerCenterOfMassPos).normalized() * leashJumpCancelScalar;
		leashJumpCancelDampTimer = LEASH_JUMP_DAMP_TIMER_MAX;
	}
}

void FPSController::UpdateHookShotTransform()
{
	cam->CalcViewMatrix();
	cam->CalcWorldMatrix();
	XMFLOAT3 camPos = cam->position;
	XMFLOAT3 camDir = cam->direction;
	XMFLOAT3 camRight = cam->right;
	XMFLOAT3 newPos = XMFLOAT3(camPos.x + -camRight.x, camPos.y + camDir.y - 0.65f, camPos.z + -camRight.z);

	XMFLOAT3 hookshotDirection;
	XMVECTOR direction;
	if (ps == PlayerState::HookshotLeash)
	{
		direction = XMVectorSubtract(XMLoadFloat3(&Utility::BulletVectorToFloat3(leashedEnemy->GetRBody()->getCenterOfMassPosition())), XMLoadFloat3(&newPos));
	}
	else
	{
		direction = XMVectorSubtract(XMLoadFloat3(&Utility::BulletVectorToFloat3(hookshotPoint)), XMLoadFloat3(&newPos));
	}

	XMStoreFloat3(&hookshotDirection, direction);
	XMStoreFloat(&hookshotLength, XMVector3Length(direction));

	hookshot->SetPosition(newPos);
	hookshot->SetDirectionVector(hookshotDirection);
	
	hookshot->SetRepeatTexture(1.0f, hookshotZScale);
	XMFLOAT3 hookshotScale = hookshot->GetScale();
	hookshotScale.z = hookshotZScale;
	hookshot->SetScale(hookshotScale);
	hookshot->CalcWorldMatrix();
}

void FPSController::ResetHookshotTransform()
{
	hookshotZScale = 0.0;
	hookshot->SetScale(1, 1, hookshotZScale);
	hookshot->CalcWorldMatrix();
	ps = PlayerState::Normal;
}

PlayerState FPSController::GetPlayerState()
{
	return ps;
}

Entity* FPSController::GetLeashedEntity()
{
	return leashedEnemy;
}

void FPSController::SetLeashedEntity(Entity* e)
{
	leashedEnemy = e;
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
	if (keyboard->KeyIsPressed(0x57)) // w
	{ 
		btVector3 forwardForce;
		if (midAir)
		{
			forwardForce = btVector3(direction.x, 0, direction.z);
		}
		else
		{
			forwardForce = btVector3(direction.x, 0, direction.z) * spd;
		}

		controllerVelocity += forwardForce;
	}
	if (keyboard->KeyIsPressed(0x53)) // s
	{ 
		btVector3 backwardForce;

		if (midAir)
		{
			backwardForce = btVector3(direction.x, 0, direction.z) * -1;
		}
		else
		{
			backwardForce = btVector3(direction.x, 0, direction.z) * -spd;
		}

		controllerVelocity += backwardForce;
	}
	if (keyboard->KeyIsPressed(0x41)) // a
	{ 
		btVector3 leftwardForce;

		if (midAir)
		{
			leftwardForce = btVector3(right.x, 0, right.z);
		}
		else
		{
			leftwardForce = btVector3(right.x, 0, right.z) * spd;
		}

		controllerVelocity += leftwardForce;
		rollRight = true;
	}
	else
	{
		rollRight = false;
	}
	if (keyboard->KeyIsPressed(0x44)) // d
	{ 
		btVector3 rightwardForce;

		if (midAir)
		{
			rightwardForce = btVector3(right.x, 0, right.z) * -1;
		}
		else
		{
			rightwardForce = btVector3(right.x, 0, right.z) * -spd;
		}

		controllerVelocity += rightwardForce;
		rollLeft = true;
	}
	else
	{
		rollLeft = false;
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


	// FORCES ADDED TO RIGIDBODY 
	playerRBody->activate();
	controllerVelocity += jumpForce;
	playerRBody->setLinearVelocity(controllerVelocity);
	playerRBody->applyCentralImpulse(impulseSumVec);

	// Damping
	DampForces();
	
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
	XMFLOAT3 pos = entity->GetPosition();
	//cout << pos.x << "|" << pos.y << "|" << pos.z << endl;
	//XMFLOAT3 dir = entity->GetDirectionVector();
	btVector3 from(pos.x, pos.y, pos.z);
	btVector3 to(pos.x, pos.y - 3.05f, pos.z); // check a little below the player for any surface to stand on 

	// Create variable to store the ray hit and set flags
	btCollisionWorld::ClosestRayResultCallback closestResult(from, to);
	closestResult.m_flags &= btTriangleRaycastCallback::kF_FilterBackfaces;

	Config::DynamicsWorld->rayTest(from, to, closestResult); // Raycast

	if (closestResult.hasHit()) // if there is a surface to stand on
	{
		// Get the entity associated with the rigid body we hit
		//Entity* hit = (Entity*)(closestResult.m_collisionObject->getUserPointer());
		//printf("Hit: %s\n", hit->GetName().c_str());*/
		PhysicsWrapper* wrapper = (PhysicsWrapper*)closestResult.m_collisionObject->getUserPointer();
		if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
			Entity* e = (Entity*)wrapper->objectPointer;
			if (e != entity) {
				//cout << e->GetName() << endl;
 				midAir = false;
				playerRBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
				jumpCount = 0;
			}
		}
	}
	else
	{
		midAir = true;
		playerRBody->setGravity(btVector3(0.0f, -25.0f, 0.0f));
	}
}

void FPSController::UpdateHeadbob()
{
	if (keyboard->CheckKeysPressed(baseMovementKeys, 4) && !midAir) // if any base movement keys are down and we are on the ground we want to headbob
	{
		if (headbobOffset < HEADBOB_OFFSET_MAX && !resetHeadbob) // increase headbob offset if it is less than the max and we are not resetting 
		{
			headbobOffset += HEADBOB_OFFSET_INTERVAL * deltaTime;
		}
		else
		{
			resetHeadbob = true; // reset headbob sice we reached the max
		}

		if (headbobOffset > HEADBOB_OFFSET_MIN && resetHeadbob) // decrease headbob offset if it is greater than the min and we are resetting
		{
			headbobOffset -= HEADBOB_OFFSET_INTERVAL * deltaTime;
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
			headbobOffset -= HEADBOB_OFFSET_INTERVAL * deltaTime;
		}
	}

	// cout << headbobOffset << endl;
}

btVector3 FPSController::JumpForceFromInput()
{
	btVector3 jumpForce = btVector3(0, 0, 0);
	if (keyboard->OnKeyDown(VK_SPACE)) {
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

		// before the timer runs out to begin damping, interpolate fov up to dash fov
		if (fov < DASH_FOV)
		{
			fov += fovNormalToDashSpeed * deltaTime;
			cam->SetFOV(fov);
		}
	}

	if (dashRegenerationTimer > 0)
	{
		dashRegenerationTimer -= deltaTime;
	}
	else if(dashCount < MAX_DASHES)
	{
		dashCount++;
		dashRegenerationTimer = DASH_MAX_REGENERATION_TIME;
	}

	btVector3 dashImpulse = btVector3(0, 0, 0);
	if (dashCount > 0 && keyboard->OnKeyDown(VK_SHIFT))
	{
		dashCount--;
		// cout << dashCount << endl;
		// default dash to forwards
		dashImpulse = btVector3(direction.x, 0, direction.z) * dashImpulseScalar;

		if (keyboard->KeyIsPressed(0x41)) // left
		{
			dashImpulse = btVector3(right.x, 0, right.z) * dashImpulseScalar;
		}
		if (keyboard->KeyIsPressed(0x44)) // right
		{
			dashImpulse = btVector3(right.x, 0, right.z) * -dashImpulseScalar;
		}
		if (keyboard->KeyIsPressed(0x53)) // backwards
		{
			dashImpulse = btVector3(direction.x, 0, direction.z) * -dashImpulseScalar;
		}

		dashDampTimer = DASH_DAMP_TIMER_MAX;

		dashBlurCallback.active = true;
	}

	return dashImpulse;
}

void FPSController::DampForces()
{
	if (dashDampTimer <= 0 && leashJumpCancelDampTimer <= 0) // always damp the impulse vec unless player is the player just initiated a dash or a leash jump cancel
	{
		dashBlurCallback.active = false;
		impulseSumVec -= impulseSumVec * dampingScalar;

		// return fov to normal when damping dash impulse
		if (fov > NORMAL_FOV)
		{
			// cout << fov << endl;
			fov -= fovDashToNormalSpeed * deltaTime;
			cam->SetFOV(fov);
		}
	}
	else if(leashJumpCancelDampTimer > 0) // Putting this here for now since its from another state and would not be able to follow the same format as the dashDampTimer
	{
		leashJumpCancelDampTimer -= deltaTime;
	}

	if (!keyboard->CheckKeysPressed(baseMovementKeys, 4) && !midAir) // Only damp overall movement if none of the base movement keys are pressed while on the ground. 
	{
		controllerVelocity -= controllerVelocity * dampingScalar;
	}
}

void FPSController::MouseLook()
{
	// update camera roll
	bool isReturning = (!rollLeft && !rollRight) || (rollLeft && rollRight);
	if (isReturning) // if side movement keys are not being pressed return to normal camera zRotation depending on what the current rotation is or if both bools are true at the same time straighten cam to avoid jittering
	{
		if (cam->zRotation > 0)
		{
			camRollAngle -= camRollSpeed * deltaTime;
		}
		else if (cam->zRotation < 0)
		{
			camRollAngle += camRollSpeed * deltaTime;
		}
		else
		{
			camRollAngle = 0;
		}
	}
	else // otherwise role to the respective min and max positions according to boolean assigned from input in Move()
	{
		if (cam->zRotation < CAM_ROLL_MAX && rollRight)
		{
			camRollAngle += camRollSpeed * deltaTime;
		}
		else if (cam->zRotation > CAM_ROLL_MIN && rollLeft)
		{
			camRollAngle -= camRollSpeed * deltaTime;
		}
		else
		{
			camRollAngle = 0;
			rollLeft = false;
			rollRight = false;
		}
	}


	cam->RotateCamera((mouse->GetPosX() - (int)prevMousePos.x) / 100.0f, (mouse->GetPosY() - (int)prevMousePos.y) / 100.0f, camRollAngle);
	if (cam->zRotation > CAM_ROLL_MAX) cam->zRotation = CAM_ROLL_MAX;
	if (cam->zRotation < CAM_ROLL_MIN) cam->zRotation = CAM_ROLL_MIN;

	if (((cam->zRotation > 0 && camRollAngle > 0) || (cam->zRotation < 0 && camRollAngle < 0)) && isReturning) cam->zRotation = 0;

	prevMousePos.x = mouse->GetPosX();
	prevMousePos.y = mouse->GetPosY();

}

void FPSController::OnCollision(btCollisionObject* other)
{
	Entity* otherE = (Entity*)((PhysicsWrapper*)other->getUserPointer())->objectPointer;
	
}