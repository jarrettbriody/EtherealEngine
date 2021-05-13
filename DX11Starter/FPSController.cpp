#include "pch.h"
#include "FPSController.h"
#include "CyclopsEnemy.h"
#include "BullEnemy.h"
#include "HornedEnemy.h"
#include "TowerEnemy.h"


void FPSController::Init()
{
	Config::ShowCursor = false;
	Config::CaptureMouse = true;
	Config::DebugCamera = false;

	if (Config::CaptureMouse) {
		RECT rect;
		if (GetWindowRect(Config::hWnd, &rect)) {
			prevMousePos.x = rect.left + Config::ViewPortWidth / 2.0f;
			prevMousePos.y = rect.top + Config::ViewPortHeight / 2.0f;
			SetCursorPos(prevMousePos.x, prevMousePos.y);
		}
	}

	while (ShowCursor(Config::ShowCursor) >= 0);


	uiDebugCb.spriteBatch = new SpriteBatch(Config::Context);
	uiDebugCb.font = new SpriteFont(Config::Device, L"../../Assets/Fonts/Bloodlust.spritefont");
	//EERenderer->SetRenderUICallback(true, &uiDebugCb, 0);

	eMap = ScriptManager::sceneEntitiesMap;
	bloodOrb = eMap->find("Blood_Orb")->second;
	bloodOrbScript = (BloodOrb*)scriptFunctionsMap[(*eMap)["Blood_Orb"]->GetName()]["BLOODORB"];
	cam = ScriptManager::EERenderer->GetCamera("main");
	cam->GetTransform().SetRotationDegrees(0, 90, 0);
	cam->rotation = cam->GetTransform().GetEulerAnglesRadians();
	direction = cam->GetTransform().GetDirectionVector();
	cam->SetFOV(fov);

	icicleParams = {
			"Blood Icicle",					// name
			"Blood Icicle",					// tag
			"Blood Icicle",					// layer
			"bloodicicle",					// mesh
			"swordgradient",				// material
			{"BLOODICICLE"},				// script names
			1,								// script count
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// position
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// rotation
			XMFLOAT3(0.5f, 1.0f, 0.5f),		// scale
			1.0f,							// mass
			true,
			BulletColliderShape::BOX
			// defaults work for the rest
	};
	

	EntityCreationParameters dashRingParams = {
			"Dash Ring",					// name
			"Dash Ring",					// tag
			"Dash Ring",					// layer
			"dashring",						// mesh
			"Red",							// material
			{""},							// script names
			0,								// script count
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// position
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// rotation
			XMFLOAT3(1.1f, 1.1f, 1.1f),		// scale
			0.0f,							// mass
			false
			// defaults work for the rest
	};

	for (int i = 0; i < MAX_DASHES; i++)
	{
		Entity* dashRing = ScriptManager::CreateEntity(dashRingParams);
		dashRings.push_back(dashRing);
		bloodOrb->GetTransform().AddChild(dashRing->GetTransformPtr(), false);
	}

	/*
	swordParams = {
			"Blood Sword",					// name
			"Blood Sword",					// tag
			"Blood Sword",					// layer
			"bloodsword",					// mesh
			"Red",							// material
			{"BLOODSWORD"},					// script names
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

	hookshotParams = {	
			"Hookshot",						// name
			"Hookshot",						// tag
			"playertools",					// layer
			"bloodchain",					// mesh
			"bloodchain",					// material
			{""},							// script names
			0,								// script count
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// position
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// rotation
			XMFLOAT3(1.0f, 1.0f, 0.0f),		// scale
			0.0f,							// mass
			false							// defaults work for the rest
	};

	hookshot = ScriptManager::CreateEntity(hookshotParams);
	//hookshot->collisionsEnabled = false;

	playerRBody = entity->GetRBody(); // Get the bullet rigidbody
	playerRBody->setAngularFactor(btVector3(0, 1, 0)); // constrain rotations on x and z axes
	// playerRBody->setLinearFactor(btVector3(1, 0, 1));
	// playerRBody->setRestitution(0.1f);
	playerRBody->setFriction(0.0f); //--> debating using friction because it would probably result in weird interations with other level gemoetry, would rather just use pure velocity dampening....UPDATE: Ended up needing to set this to zero because it would cause the character to wall ride

	keyboard = Keyboard::GetInstance();
	mouse = Mouse::GetInstance();

	controllerVelocity = btVector3(0, 0, 0);
	impulseSumVec = btVector3(0, 0, 0);
	hookshotPoint = btVector3(0, 0, 0);

	collider = entity->GetCollider();

	ps = PlayerState::Normal;

	dashBlurCallback.vShader = EESceneLoader->VertexShadersMap["PostProcess"];
	dashBlurCallback.pShader = EESceneLoader->PixelShadersMap["DashBlur"];
	dashBlurCallback.layerMask = EERenderer->depthStencilComponents.entityInfoSRV;
	EERenderer->SetPostProcess(true, &dashBlurCallback, 1);

	ParticleEmitterDescription emitDesc;
	emitDesc.emitterPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	emitDesc.emitterDirection = NEG_Z_AXIS;
	emitDesc.colorCount = 3;
	ParticleColor particleColors[3] = {
		{XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), 1.0f},
		{XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), 1.0f},
		{XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f), 1.0f},
	};
	emitDesc.colors = particleColors;
	emitDesc.bakeWorldMatOnEmission = true;
	emitDesc.emissionStartRadius = 0.1f;
	emitDesc.emissionEndRadius = 1.0f;
	emitDesc.emissionRate = 3000.0;
	emitDesc.maxParticles = 3000;
	emitDesc.particleInitMinSpeed = 5.0f;
	emitDesc.particleInitMaxSpeed = 10.0f;
	emitDesc.particleMinLifetime = 4.0f;
	emitDesc.particleMaxLifetime = 6.0f;
	emitDesc.particleInitMinScale = 0.01f;
	emitDesc.particleInitMaxScale = 0.2f;
	//emitDesc.fadeInEndTime = 0.1f;
	//emitDesc.fadeIn = true;
	emitDesc.fadeOutStartTime = 0.5f;
	emitDesc.fadeOut = true;
	emitDesc.particleAcceleration = XMFLOAT3(0, -35.0f, 0);

	hookshotEmitter = new GPUParticleEmitter(emitDesc);

	for (size_t i = 0; i < 4; i++)
	{
		float x = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) * XM_2PI;
		float y = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) * XM_2PI;
		float z = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) * XM_2PI;

		XMFLOAT3 xA = X_AXIS;
		XMFLOAT3 yA = Y_AXIS;
		XMFLOAT3 zA = Z_AXIS;

		XMVECTOR rollQuat = XMQuaternionRotationAxis(XMLoadFloat3(&zA), z);
		XMVECTOR yawQuat = XMQuaternionRotationAxis(XMLoadFloat3(&yA), y);
		XMVECTOR tiltQuat = XMQuaternionRotationAxis(XMLoadFloat3(&xA), x);
		XMVECTOR resultQuat = XMQuaternionNormalize(XMQuaternionMultiply(XMQuaternionMultiply(yawQuat, tiltQuat), rollQuat));

		XMStoreFloat4(&ringRandomQuats[i], resultQuat);

		dashRingSpd[i] = 0.01f;
	}

	Config::FMODResult = Config::FMODSystem->playSound(Config::Footstep, Config::SFXGroup, true, &footstepChannel);
	footstepChannel->setVolume(FOOTSTEP_VOLUME);
	XMFLOAT3 epos = entity->GetTransform().GetPosition();
	FMOD_VECTOR pos = { epos.x,epos.y, epos.z };
	FMOD_VECTOR vel = { 0, 0, 0 };

	footstepChannel->set3DAttributes(&pos, &vel);
	footstepChannel->set3DMinMaxDistance(0, 8.0f);
}

void FPSController::Update()
{

	XMFLOAT3 playerPos = entity->GetTransform().GetPosition();
	std::string playerPosString;
	playerPosString += "Player Position: " + std::to_string(playerPos.x) + " | " + std::to_string(playerPos.y) + " | " + std::to_string(playerPos.z);
	uiDebugCb.playerPos = playerPosString;

	((ParticleEmitter*)hookshotEmitter)->SetIsActive(false);
	XMFLOAT3 ePos = entity->GetTransform().GetPosition();
	XMFLOAT3 eScl = entity->GetTransform().GetScale();

	if (keyboard->KeyIsPressed(0x4A)) // J
	{
		cout << "Player POS- X: " << ePos.x << " | Y: " << ePos.y << " | Z: " << ePos.z << endl;
	}

	if (bloodResource <= 0) // death
	{
		ps = PlayerState::Death;
	}

	bloodOrbScript->SetFillLinePercentage(bloodResource);

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
			cam->GetTransform().SetPosition(XMFLOAT3(ePos.x, ePos.y + eScl.y + headbobOffset, ePos.z)); // after all updates make sure camera is following the affected entity
			UpdateDashRingsTransforms();
			CheckAllAbilities();
			UpdateSwordSway();
			break;
		
		case PlayerState::HookshotThrow:
			HookshotThrow();
			MouseLook();
			cam->GetTransform().SetPosition(XMFLOAT3(ePos.x, ePos.y + eScl.y + headbobOffset, ePos.z)); // after all updates make sure camera is following the affected entity
			UpdateHookShotTransform();
			UpdateDashRingsTransforms();
			CheckBloodSword();
			CheckBloodIcicle();
			CheckBulletTime();
			UpdateSwordSway();
			break;

		case PlayerState::HookshotFlight:
			HookshotFlight();
			MouseLook();
			cam->GetTransform().SetPosition(XMFLOAT3(ePos.x, ePos.y + eScl.y + headbobOffset, ePos.z)); // after all updates make sure camera is following the affected entity
			UpdateHookShotTransform();
			UpdateDashRingsTransforms();
			CheckBloodSword();
			CheckBloodIcicle();
			CheckBulletTime();
			UpdateSwordSway();
			break;

		case PlayerState::HookshotLeash:
			HookshotLeash();
			Move();
			MouseLook();
			cam->GetTransform().SetPosition(XMFLOAT3(ePos.x, ePos.y + eScl.y + headbobOffset, ePos.z)); // after all updates make sure camera is following the affected entity
			UpdateHookShotTransform();
			UpdateDashRingsTransforms();
			CheckBloodSword();
			CheckBloodIcicle();
			CheckBulletTime();
			UpdateSwordSway();
			break;

		case PlayerState::Paused:

			break;

		case PlayerState::Death:
			// get rid of all tools
			if (sword->renderObject) // simple check because if the sword is rendered then death is just starting and we only want to do some state things once
			{
				sword->renderObject = false;
				hookshot->renderObject = false;
				bloodOrb->renderObject = false;
				EESceneLoader->SceneEntitiesMap["Blood_Orb_Glass"]->renderObject = false;

				for each (Entity * ring in dashRings)
				{
					ring->renderObject = false;
				}

				// ragdoll the player
				playerRBody->setAngularFactor(btVector3(1, 1, 1)); // free rotations on x and z axes
				playerRBody->setGravity(btVector3(0.0f, -30.0f, 0.0f));
				playerRBody->setMassProps(10, btVector3(0, 0, 0));
				playerRBody->setFriction(1.0f);
				//playerRBody->applyImpulse(btVector3(5, 2, 10), btVector3(0, playerRBody->getCenterOfMassPosition().getY() - entity->GetScale().y, 0));
				playerRBody->applyTorqueImpulse(btVector3(0,-2, 0)); // fall back

				DXCore::deltaTimeScalar = 0.75;
				/*sword->InitRigidBody(BulletColliderShape::BOX, 1.0f);
				sword->GetRBody()->setGravity(btVector3(0, -25, 0));

				bloodOrb->InitRigidBody(BulletColliderShape::CAPSULE, 1.0f);
				bloodOrb->GetRBody()->setGravity(btVector3(0, -25, 0));*/
			}

			cam->GetTransform().SetPosition(XMFLOAT3(entity->GetTransform().GetPosition().x, entity->GetTransform().GetPosition().y + entity->GetTransform().GetScale().y + headbobOffset, entity->GetTransform().GetPosition().z)); // after all updates make sure camera is following the affected entity
			float rotX;
			float rotY;
			float rotZ;
			playerRBody->getCenterOfMassTransform().getRotation().getEulerZYX(rotX, rotY, rotZ);

			cam->RotateCamera(rotX * deltaTime, rotY * deltaTime, rotZ * deltaTime);
	
			break;

		case PlayerState::Victory:
			
			break;

		default:

			break;
	}


	XMFLOAT3 epos = entity->GetTransform().GetPosition();
	FMOD_VECTOR pos = { epos.x,epos.y - 3.0f, epos.z };
	FMOD_VECTOR vel = { 0, 0, 0 };

	footstepChannel->set3DAttributes(&pos, &vel);
	footstepChannel->set3DMinMaxDistance(0, 8.0f);
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


			//Config::MusicChannel->set3DAttributes(&pos, &vel);
			//Config::MusicChannel->set3DMinMaxDistance(0, 15.0f);
	}
}


void FPSController::CheckBloodIcicle()
{
	if (mouse->OnRMBDown() && bloodIcicleCooldownTimer <= 0) 
	{
		bloodResource -= 20;

		// update position and rotation of the EntityCreationParams
		icicleParams.position = bloodOrb->GetTransform().GetPosition();
		icicleParams.rotationRadians = XMFLOAT3(cam->rotation.x + 1.5708f /* 90 degress in radians */ , cam->rotation.y, cam->rotation.z);

		Entity* bloodIcicle = ScriptManager::CreateEntity(icicleParams);

		btVector3 shotImpulse = Utility::Float3ToBulletVector(cam->GetTransform().GetDirectionVector());

		bloodIcicle->GetRBody()->setGravity(btVector3(0,0,0));
		bloodIcicle->GetRBody()->activate();
		bloodIcicle->GetRBody()->applyCentralImpulse(shotImpulse.normalized() * bloodIcicleScalar);

		// backwards recoil impulse on player
		impulseSumVec += btVector3(-direction.x, 0, -direction.z).normalized() * bloodIcicleRecoilScalar;

		bloodIcicleRecoilDampTimer = BLOOD_ICICLE_RECOIL_DAMP_TIMER_MAX;

		bloodIcicleCooldownTimer = BLOOD_ICICLE_MAX_COOLDOWN_TIME;

		int index = (rand() % 3) + 6;
		Config::FMODResult = Config::FMODSystem->playSound(Config::Icicle[index], Config::SFXGroup2D, false, &Config::SFXChannel2D);
		Config::SFXChannel2D->setVolume(ICICLE_THROW_VOLUME);
	}
	else if(bloodIcicleCooldownTimer > 0)
	{
		bloodIcicleCooldownTimer -= deltaTime;
	}
}

void FPSController::CheckBulletTime()
{
	// Slow time instantly and keep it slowed while Q is pressed but gradually ramp time back up to normal time when not pressed 
	if (keyboard->OnKeyDown(0x51)) 
	{
		bloodResource -= 25;

		bulletTimeRampDownTimer = BULLET_TIME_RAMP_DOWN_TIMER_MAX;

		bulletTimeRampDown = BULLET_TIME_SCALAR;
		DXCore::deltaTimeScalar = BULLET_TIME_SCALAR;
	}
	else if(bulletTimeRampDownTimer <= 0)
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
	else
	{
		bulletTimeRampDownTimer -= deltaTime;
	}
}

void FPSController::CheckHookshot()
{
	if (keyboard->OnKeyDown(0x45)) // E
	{
		Config::DynamicsWorld->updateAabbs();
		Config::DynamicsWorld->computeOverlappingPairs();

		// Redefine our vectors using bullet's types
		btVector3 from(Utility::Float3ToBulletVector(bloodOrb->GetTransform().GetPosition()));
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
			hookshotHitNormal = closestResult.m_hitNormalWorld;

			if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY) 
			{
				// Entity* hit = (Entity*)wrapper->objectPointer;
				hookshotAttachedEntity = (Entity*)wrapper->objectPointer;

				ps = PlayerState::HookshotThrow;

				Config::FMODResult = Config::FMODSystem->playSound(Config::Hookshot[0], Config::SFXGroup2D, false, &Config::SFXChannel2D);
				Config::SFXChannel2D->setVolume(HOOKSHOT_THROW_VOLUME);

				Config::FMODResult = Config::FMODSystem->playSound(Config::Hookshot[1], Config::SFXGroup2D, false, &hookshotChannel);
				hookshotChannel->setVolume(HOOKSHOT_EXTENDING_VOLUME);
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
		hookshotChannel->stop();

		if (hookshotAttachedEntity->HasTag("Enemy") || hookshotAttachedEntity->HasTag("Body Part"))
		{
			leashedEnemy = hookshotAttachedEntity;
			leashSize = playerRBody->getCenterOfMassPosition().distance(leashedEnemy->GetRBody()->getCenterOfMassPosition()); 
			// cout << leashSize << endl;

			// handle each specific enemy scripts response to leashing if it alive by storing it in a simple abstract base type they all inherit
			if (hookshotAttachedEntity->HasTag("Enemy"))
			{
				if (hookshotAttachedEntity->HasTag("Cyclops"))
				{
					enemyScript = (CyclopsEnemy*)scriptFunctionsMap[leashedEnemy->GetName()]["CYCLOPSENEMY"];
				}

				if (hookshotAttachedEntity->HasTag("Bull"))
				{
					enemyScript = (BullEnemy*)scriptFunctionsMap[leashedEnemy->GetName()]["BULLENEMY"];
				}

				if (hookshotAttachedEntity->HasTag("Horned"))
				{
					enemyScript = (HornedEnemy*)scriptFunctionsMap[leashedEnemy->GetName()]["HORNEDENEMY"];
				}

				if (hookshotAttachedEntity->HasTag("Tower"))
				{
					enemyScript = (TowerEnemy*)scriptFunctionsMap[leashedEnemy->GetName()]["TOWERENEMY"];
				}

				enemyScript->IsLeashed(true, 0.0f);
			}

			ps = PlayerState::HookshotLeash;
		}
		else if (hookshotAttachedEntity->HasTag("Environment"))
		{
			// playerRBody->clearForces(); --> don't know if needed
			((ParticleEmitter*)hookshotEmitter)->SetIsActive(true);
			((ParticleEmitter*)hookshotEmitter)->SetPosition(Utility::BulletVectorToFloat3(hookshotPoint));
			((ParticleEmitter*)hookshotEmitter)->SetDirectionVector(Utility::BulletVectorToFloat3(hookshotHitNormal));

			ps = PlayerState::HookshotFlight;

			Config::FMODResult = Config::FMODSystem->playSound(Config::Hookshot[2], Config::SFXGroup, false, &Config::SFXChannel);
			Config::SFXChannel->setVolume(HOOKSHOT_IMPACT_VOLUME);
			FMOD_VECTOR pos = { hookshotPoint.getX(), hookshotPoint.getY(), hookshotPoint.getZ() };
			FMOD_VECTOR vel = { 0, 0, 0 };

			Config::SFXChannel->set3DAttributes(&pos, &vel);
			Config::SFXChannel->set3DMinMaxDistance(0, 100.0f);

			Config::FMODResult = Config::FMODSystem->playSound(Config::Hookshot[3], Config::SFXGroup, false, &hookshotChannel);
			hookshotChannel->setVolume(HOOKSHOT_REEL_VOLUME);
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
			hookshotChannel->stop();
			Config::FMODResult = Config::FMODSystem->playSound(Config::Hookshot[4], Config::SFXGroup2D, false, &Config::SFXChannel2D);
			Config::SFXChannel2D->setVolume(HOOKSHOT_RELEASE_VOLUME);
		}
	}
	else // cancel if not holding E
	{
		ResetHookshotTransform();
		hookshotChannel->stop();
		Config::FMODResult = Config::FMODSystem->playSound(Config::Hookshot[4], Config::SFXGroup2D, false, &Config::SFXChannel2D);
		Config::SFXChannel2D->setVolume(HOOKSHOT_RELEASE_VOLUME);
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
		if (hookshotAttachedEntity->HasTag("Enemy")) enemyScript->IsLeashed(false, 2.0f);

		ResetHookshotTransform();

		// Pull enemy towards player when canceling the leash
		leashedEnemy->GetRBody()->applyCentralImpulse((playerCenterOfMassPos - leashedEnemy->GetRBody()->getCenterOfMassPosition()).normalized() * leashCancelScalar * (1 / leashedEnemy->GetRBody()->getInvMass()));
	}

	if (keyboard->OnKeyDown(VK_SPACE)) // pull player to enemy cancel after press space 
	{
		if (hookshotAttachedEntity->HasTag("Enemy")) enemyScript->IsLeashed(false, 0.5f);

		ResetHookshotTransform();

		// Pull player towards enemy when canceling the leash
		// Not scaling Y for now because the upwards force makes you shoot into the sky and damping doesn't take care of it quick enough, think it hwas to do with the change from zero gravity when grounded to gravity after grouned is false
		btVector3 jumpCancelImpulse = (leashedEnemy->GetRBody()->getCenterOfMassPosition() - playerCenterOfMassPos).normalized();
		jumpCancelImpulse.setX(jumpCancelImpulse.getX() * leashJumpCancelScalar);
		jumpCancelImpulse.setZ(jumpCancelImpulse.getZ() * leashJumpCancelScalar);
		impulseSumVec += jumpCancelImpulse;
		leashJumpCancelDampTimer = LEASH_JUMP_DAMP_TIMER_MAX;
	}
}

void FPSController::UpdateHookShotTransform()
{
	cam->CalcViewMatrix();
	//cam->CalcWorldMatrix();
	XMFLOAT3 camPos = cam->GetTransform().GetPosition();
	XMFLOAT3 camDir = cam->GetTransform().GetDirectionVector();
	XMFLOAT3 camRight = cam->GetTransform().GetRightVector();
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

	hookshot->GetTransform().SetPosition(newPos);
	//XMFLOAT3 up;
	//XMStoreFloat3(&up, XMVectorSubtract(XMLoadFloat3(&camPos), XMLoadFloat3(&newPos)));
	//hookshot->SetDirectionVectorU(hookshotDirection, up);
	hookshot->GetTransform().SetDirectionVector(hookshotDirection);
	hookshot->GetTransform().RotateAroundAxis(Z_AXIS, XMConvertToRadians(-45.0f));
	
	hookshot->SetRepeatTexture(1.0f, hookshotZScale);
	XMFLOAT3 hookshotScale = hookshot->GetTransform().GetScale();
	hookshotScale.z = hookshotZScale;
	hookshot->GetTransform().SetScale(hookshotScale);
}

void FPSController::UpdateDashRingsTransforms()
{
	totalTime += deltaTime;
	cam->CalcViewMatrix();
	//cam->CalcWorldMatrix();
	XMFLOAT3 camPos = cam->GetTransform().GetPosition();
	XMFLOAT3 camDir = cam->GetTransform().GetDirectionVector();
	XMFLOAT3 camRight = cam->GetTransform().GetRightVector();

	/*
	for (size_t i = 0; i < 4; i++)
	{
		XMVECTOR current = XMLoadFloat4(&dashRings[i]->GetTransform().GetRotationQuaternion());
		XMVECTOR to = XMLoadFloat4(&ringRandomQuats[i]);
		XMFLOAT4 quat;
		XMVECTOR newQuat = XMQuaternionSlerp(current, to, dashRingSpd[i] * deltaTime);
		XMStoreFloat4(&quat, newQuat);
		dashRings[i]->GetTransform().SetRotationQuaternion(quat);
		dashRings[i]->GetTransform().SetPosition(XMFLOAT3(camPos.x + camDir.x * 1.1f, camPos.y + camDir.y - 0.5f, camPos.z + camDir.z * 1.1f));

		dashRingSpd[i] += 1.0f * deltaTime;
		if (dashRingSpd[i] > 0.5f) dashRingSpd[i] = 0.5f;

		float dot = XMQuaternionDot(newQuat, to).m128_f32[0];
		if (dot > 0.999f && dot < 1.001f) {
			float x = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) * XM_2PI;
			float y = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) * XM_2PI;
			float z = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) * XM_2PI;

			XMFLOAT3 xA = X_AXIS;
			XMFLOAT3 yA = Y_AXIS;
			XMFLOAT3 zA = Z_AXIS;

			XMVECTOR rollQuat = XMQuaternionRotationAxis(XMLoadFloat3(&zA), z);
			XMVECTOR yawQuat = XMQuaternionRotationAxis(XMLoadFloat3(&yA), y);
			XMVECTOR tiltQuat = XMQuaternionRotationAxis(XMLoadFloat3(&xA), x);
			XMVECTOR resultQuat = XMQuaternionNormalize(XMQuaternionMultiply(XMQuaternionMultiply(yawQuat, tiltQuat), rollQuat));

			XMStoreFloat4(&ringRandomQuats[i], resultQuat);

			dashRingSpd[i] = 0.01f;
		}
	}
	*/

	float xDegrees;
	float yDegrees;
	float zDegrees;

	// dash ring updating position and spinning
	//dashRings[0]->GetTransform().SetPosition(XMFLOAT3(camPos.x + camDir.x * 1.1f, camPos.y + camDir.y - 0.5f, camPos.z + camDir.z * 1.1f));
	//dashRings[0]->GetTransform().SetPosition(bloodOrb->GetTransform().GetPosition());
	xDegrees = dashRings[0]->GetTransform().GetEulerAnglesRadians().x;
	dashRings[0]->GetTransform().SetRotationRadians(Utility::FloatLerp(xDegrees, xDegrees + 10, 0.05 * deltaTime), 0, 0);
	
	//dashRings[1]->GetTransform().SetPosition(XMFLOAT3(camPos.x + camDir.x * 1.1f, camPos.y + camDir.y - 0.5f, camPos.z + camDir.z * 1.1f));
	//dashRings[1]->GetTransform().SetPosition(bloodOrb->GetTransform().GetPosition());
	xDegrees = dashRings[1]->GetTransform().GetEulerAnglesRadians().x;
	dashRings[1]->GetTransform().SetRotationRadians(Utility::FloatLerp(xDegrees, xDegrees - 10, 0.15 * deltaTime), 0, 0);
	
	//dashRings[2]->GetTransform().SetPosition(XMFLOAT3(camPos.x + camDir.x * 1.1f, camPos.y + camDir.y - 0.5f, camPos.z + camDir.z * 1.1f));
	//dashRings[2]->GetTransform().SetPosition(bloodOrb->GetTransform().GetPosition());
	zDegrees = dashRings[2]->GetTransform().GetEulerAnglesRadians().z;
	dashRings[2]->GetTransform().SetRotationRadians(0, 0, Utility::FloatLerp(zDegrees, zDegrees + 10, 0.05 * deltaTime));
	
	//dashRings[3]->GetTransform().SetPosition(XMFLOAT3(camPos.x + camDir.x * 1.1f, camPos.y + camDir.y - 0.5f, camPos.z + camDir.z * 1.1f));
	//dashRings[3]->GetTransform().SetPosition(bloodOrb->GetTransform().GetPosition());
	zDegrees = dashRings[3]->GetTransform().GetEulerAnglesRadians().z;
	dashRings[3]->GetTransform().SetRotationRadians(0, 0, Utility::FloatLerp(zDegrees, zDegrees - 10, 0.15 * deltaTime));
}

void FPSController::ResetHookshotTransform()
{
	hookshotZScale = 0.0;
	hookshot->GetTransform().SetScale(1, 1, hookshotZScale);
	ps = PlayerState::Normal;
}

FPSController::~FPSController()
{
	delete uiDebugCb.spriteBatch;
	delete uiDebugCb.font;
}


PlayerState FPSController::GetPlayerState()
{
	return ps;
}

void FPSController::UpdateDashRingsActive(bool setActive)
{
	for each (Entity* orb in dashRings)
	{
		if (setActive) // if true we want to set the first non-rendered dash orb we find to display
		{
			if (!orb->renderObject)
			{
				orb->renderObject = true;
				return;
			}
		}
		else // otherwise find the first rendered dash orb to hide
		{
			if (orb->renderObject)
			{
				orb->renderObject = false;
				return;
			}
		}
	}
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
	direction = cam->GetTransform().GetDirectionVector();
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
			footstepChannel->setPaused(true);
		}
		else
		{
			forwardForce = btVector3(direction.x, 0, direction.z) * spd;

			footstepChannel->setPaused(false);
		}

		controllerVelocity += forwardForce;

		swordRollForwards = true;
	}
	else {
		swordRollForwards = false;
	}
	if (keyboard->KeyIsPressed(0x53)) // s
	{ 
		btVector3 backwardForce;

		if (midAir)
		{
			backwardForce = btVector3(direction.x, 0, direction.z) * -1;
			footstepChannel->setPaused(true);
		}
		else
		{
			backwardForce = btVector3(direction.x, 0, direction.z) * -spd;

			footstepChannel->setPaused(false);
		}

		controllerVelocity += backwardForce;

		swordRollBackwards = true;
	}
	else {
		swordRollBackwards = false;
	}
	if (keyboard->KeyIsPressed(0x41)) // a
	{ 
		btVector3 leftwardForce;

		if (midAir)
		{
			leftwardForce = btVector3(right.x, 0, right.z);
			footstepChannel->setPaused(true);
		}
		else
		{
			leftwardForce = btVector3(right.x, 0, right.z) * spd;
			footstepChannel->setPaused(false);
		}

		controllerVelocity += leftwardForce;

		rollRight = true;
		swordRollRight = true;
	}
	else
	{
		rollRight = false;
		swordRollRight = false;
	}
	if (keyboard->KeyIsPressed(0x44)) // d
	{ 
		btVector3 rightwardForce;

		if (midAir)
		{
			rightwardForce = btVector3(right.x, 0, right.z) * -1;
			footstepChannel->setPaused(true);
		}
		else
		{
			rightwardForce = btVector3(right.x, 0, right.z) * -spd;

			footstepChannel->setPaused(false);
		}

		controllerVelocity += rightwardForce;

		rollLeft = true;
		swordRollLeft = true; 
	}
	else
	{
		rollLeft = false;
		swordRollLeft = false;
	}

	if (!swordRollLeft && !swordRollRight && !swordRollForwards && !swordRollBackwards) {
		footstepChannel->setPaused(true);
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
	XMFLOAT3 eulers = entity->GetTransform().GetEulerAnglesRadians();
	eulers = XMFLOAT3(0.0f, eulers.y, 0.0f);
	entity->GetTransform().SetRotationRadians(eulers);

	// cout << deltaTime << endl;
	// cout << "Controller Velocity: (" << controllerVelocity.getX() << ", " << controllerVelocity.getY() << ", " << controllerVelocity.getZ() << ")" << endl;
}

void FPSController::GroundCheck()
{
	// Ground check
	Config::DynamicsWorld->updateAabbs();
	Config::DynamicsWorld->computeOverlappingPairs();

	// Redefine our vectors using bullet's silly types
	XMFLOAT3 pos = entity->GetTransform().GetPosition();
	//cout << pos.x << "|" << pos.y << "|" << pos.z << endl;
	//XMFLOAT3 dir = entity->GetDirectionVector();
	btVector3 from(pos.x, pos.y, pos.z);
	btVector3 to(pos.x, pos.y - 3.01f, pos.z); // check a little below the player for any surface to stand on 

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
		playerRBody->setGravity(btVector3(0.0f, -30.0f, 0.0f));
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

void FPSController::UpdateSwordSway()
{
	BloodSword* swordScript = (BloodSword*)(scriptFunctionsMap["Blood Sword"]["BLOODSWORD"]);

	if (swordScript->animReset) {
		swordRoll = 0.0f;
		swordTilt = 0.0f;
	}

	if (swordRollRight && swordRoll < MAX_SWORD_ROT) {
		swordRoll += swordRotationSpeed * deltaTime;
		if (swordRoll > MAX_SWORD_ROT) swordRoll = MAX_SWORD_ROT;
	}
	if (swordRollLeft && swordRoll > -MAX_SWORD_ROT) {
		swordRoll -= swordRotationSpeed * deltaTime;
		if (swordRoll < -MAX_SWORD_ROT) swordRoll = -MAX_SWORD_ROT;
	}
	if (swordRollForwards && swordTilt < MAX_SWORD_ROT) {
		swordTilt += swordRotationSpeed * deltaTime;
		if (swordTilt > MAX_SWORD_ROT) swordTilt = MAX_SWORD_ROT;
	}
	if (swordRollBackwards && swordTilt > -MAX_SWORD_ROT) {
		swordTilt -= swordRotationSpeed * deltaTime;
		if (swordTilt < -MAX_SWORD_ROT) swordTilt = -MAX_SWORD_ROT;
	}
	if (!swordRollRight && !swordRollLeft) {
		swordRoll *= 0.95f;
	}
	if (!swordRollBackwards && !swordRollForwards) {
		swordTilt *= 0.95f;
	}

	XMFLOAT3 x = X_AXIS;
	XMFLOAT3 y = Y_AXIS;
	XMFLOAT3 z = Z_AXIS;
	XMVECTOR current = XMLoadFloat4(&sword->GetTransform().GetRotationQuaternion());
	XMVECTOR rollQuat = XMQuaternionRotationAxis(XMLoadFloat3(&z), swordRoll);
	XMVECTOR yawQuat = XMQuaternionRotationAxis(XMLoadFloat3(&y), 0.0f);
	XMVECTOR tiltQuat = XMQuaternionRotationAxis(XMLoadFloat3(&x), swordTilt);
	XMVECTOR resultQuat = XMVector4Normalize(XMQuaternionMultiply(XMQuaternionMultiply(yawQuat, tiltQuat), rollQuat));
	XMFLOAT4 quat;
	XMStoreFloat4(&quat, XMQuaternionSlerp(current, resultQuat, swordRotationSpeed * deltaTime));
	sword->GetTransform().SetRotationQuaternion(quat);
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
			int index = (rand() % 6);
			Config::FMODResult = Config::FMODSystem->playSound(Config::Jump[index], Config::SFXGroup2D, false, &Config::SFXChannel2D);
			Config::SFXChannel2D->setVolume(JUMP_VOLUME);
		}
	}

	return jumpForce;
}

btVector3 FPSController::DashImpulseFromInput()
{
	// dash regeneration
	if (dashCount < MAX_DASHES)
	{
		if (dashRegenerationTimer > 0)
		{
			dashRegenerationTimer -= deltaTime;
		}
		else
		{
			dashCount++;
			UpdateDashRingsActive(true);
			dashRegenerationTimer = DASH_MAX_REGENERATION_TIME;
			// cout << "Dash Count: " << dashCount << endl;
		}
	}
	else
	{
		dashRegenerationTimer = DASH_MAX_REGENERATION_TIME;
	}

	// dash force
	btVector3 dashImpulse = btVector3(0, 0, 0);
	if (dashCount > 0 && keyboard->OnKeyDown(VK_SHIFT))
	{
		dashCount--;
		UpdateDashRingsActive(false);

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

		int index = (rand() % 6);
		Config::FMODResult = Config::FMODSystem->playSound(Config::Dash[index], Config::SFXGroup2D, false, &Config::SFXChannel2D);
		Config::SFXChannel2D->setVolume(DASH_VOLUME);
	}

	return dashImpulse;
}

void FPSController::DampForces()
{
	if (dashDampTimer <= 0 && leashJumpCancelDampTimer <= 0 && bloodIcicleRecoilDampTimer <= 0 && onHitDampTimer <= 0) // always damp the impulse vec unless player is the player just initiated a dash or a leash jump cancel or blood icicle shot
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
	else  
	{
		if (leashJumpCancelDampTimer > 0) leashJumpCancelDampTimer -= deltaTime;
		if (bloodIcicleRecoilDampTimer > 0) bloodIcicleRecoilDampTimer -= deltaTime;
		if (onHitDampTimer > 0) onHitDampTimer -= deltaTime;
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
		if (cam->rotation.z > 0)
		{
			camRollAngle -= camRollSpeed * deltaTime;
		}
		else if (cam->rotation.z < 0)
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
		if (cam->rotation.z < CAM_ROLL_MAX && rollRight)
		{
			camRollAngle += camRollSpeed * deltaTime;
		}
		else if (cam->rotation.z > CAM_ROLL_MIN && rollLeft)
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

	POINT cursorPos;
	GetCursorPos(&cursorPos);
	cam->RotateCamera(((float)cursorPos.x - prevMousePos.x) * Config::MouseSensitivity, ((float)cursorPos.y - prevMousePos.y) * Config::MouseSensitivity, camRollAngle);

	if (Config::CaptureMouse && GetActiveWindow() == Config::hWnd) {
		RECT rect;
		if (GetWindowRect(Config::hWnd, &rect)) {
			prevMousePos.x = rect.left + Config::ViewPortWidth / 2.0f;
			prevMousePos.y = rect.top + Config::ViewPortHeight / 2.0f;
			SetCursorPos(prevMousePos.x, prevMousePos.y);
		}
	}

	if (cam->rotation.z > CAM_ROLL_MAX) cam->rotation.z = CAM_ROLL_MAX;
	if (cam->rotation.z < CAM_ROLL_MIN) cam->rotation.z = CAM_ROLL_MIN;

	if (((cam->rotation.z > 0 && camRollAngle > 0) || (cam->rotation.z < 0 && camRollAngle < 0)) && isReturning) cam->rotation.z = 0;

	//prevMousePos.x = mouse->GetPosX();
	//prevMousePos.y = mouse->GetPosY();

}

void FPSController::OnCollision(btCollisionObject* other)
{
	PhysicsWrapper* pWrap = (PhysicsWrapper*)other->getUserPointer();

	if (pWrap->type != PHYSICS_WRAPPER_TYPE::ENTITY) return;

	Entity* otherE = (Entity*)((PhysicsWrapper*)other->getUserPointer())->objectPointer;

	if (otherE->HasTag(std::string("Blood Pool")))
	{
		bloodResource += 10;
		otherE->Destroy();
	}

	if (otherE->HasTag(std::string("cyclopsProjectile")))
	{
		bloodResource -= 10;
		impulseSumVec += Utility::Float3ToBulletVector(otherE->GetTransform().GetDirectionVector()).normalized() * 100.0f;
		otherE->Destroy();

		int index = (rand() % 10);
		Config::FMODResult = Config::FMODSystem->playSound(Config::PlayerHit[index], Config::SFXGroup2D, false, &Config::SFXChannel2D);
		Config::SFXChannel2D->setVolume(PLAYER_HIT_VOLUME);
	}

	if (otherE->HasTag(std::string("towerProjectile")))
	{
		bloodResource -= 25;
		impulseSumVec += Utility::Float3ToBulletVector(otherE->GetTransform().GetDirectionVector()).normalized() * 500.0f;
		otherE->Destroy();
		int index = (rand() % 10);
		Config::FMODResult = Config::FMODSystem->playSound(Config::PlayerHit[index], Config::SFXGroup2D, false, &Config::SFXChannel2D);
		Config::SFXChannel2D->setVolume(PLAYER_HIT_VOLUME);
	}

	if (otherE->HasTag(std::string("Enemy")) && onHitDampTimer <= 0)
	{
		
		if (otherE->HasTag(std::string("Horned")))
		{
			bloodResource -= 10;
			impulseSumVec += Utility::Float3ToBulletVector(otherE->GetTransform().GetDirectionVector()).normalized() * 40.0f;
		}

		if (otherE->HasTag(std::string("Bull")))
		{
			bloodResource -= 10; // this is a more constant ten because the bull keeps on the player

			XMFLOAT3 pushDir = entity->GetTransform().GetRightVector();
			otherE->GetRBody()->activate();
			impulseSumVec += Utility::Float3ToBulletVector(entity->GetTransform().GetRightVector()).normalized() * 20.0f;
		}

		if (otherE->HasTag(std::string("Cyclops")))
		{
			bloodResource -= 3;
		}

		onHitDampTimer = ON_HIT_DAMP_TIMER_MAX;

		int index = (rand() % 10);
		Config::FMODResult = Config::FMODSystem->playSound(Config::PlayerHit[index], Config::SFXGroup2D, false, &Config::SFXChannel2D);
		Config::SFXChannel2D->setVolume(PLAYER_HIT_VOLUME);
	}
}