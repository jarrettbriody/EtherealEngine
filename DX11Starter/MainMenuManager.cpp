#include "pch.h"
#include "MainMenuManager.h"

void MainMenuManager::Init()
{
	cam = EERenderer->GetCamera("main");
	cam->GetTransform().SetPosition(positions[0]);
	cam->GetTransform().SetRotationRadians(rotations[0]);
	cam->rotation = rotations[0];
	cam->CalcViewMatrix();

	uiCb.spriteBatch = new SpriteBatch(Config::Context);
	uiCb.font = new SpriteFont(Config::Device, L"../../Assets/Fonts/Bloodlust.spritefont");
	uiCb.EEMouse = Mouse::GetInstance();
	uiCb.EESceneLoader = EESceneLoader;
	EERenderer->SetRenderUICallback(true, &uiCb, 0);

	lights = LightHandler::GetInstance();
	hearthLight = lights->GetLight("hearth");
	hearthPtLight = lights->GetLight("hearthpoint");
	menuLight = lights->GetLight("menulight");

	originalHearth = hearthLight->light;
	originalMenu = menuLight->light;

	papers = EESceneLoader->SceneEntitiesTagMap["paper"];
	numPapers = papers.size();

	mouse = Mouse::GetInstance();

	Entity* bloodsword = EESceneLoader->SceneEntitiesMap["bloodswordlowpoly2"];

	ParticleEmitterDescription emitDesc;
	emitDesc.emitterPosition = bloodsword->GetTransform().GetPosition();
	emitDesc.emitterPosition.x = emitDesc.emitterPosition.x + 3.5f;
	emitDesc.emitterDirection = NEG_Y_AXIS; 
	emitDesc.colorCount = 1;
	ParticleColor particleColors[1] = {
		{XMFLOAT4(0.3f, 0.0f, 0.0f, 0.6f), 1.0f},
	};
	emitDesc.colors = particleColors;
	emitDesc.bakeWorldMatOnEmission = true;
	emitDesc.fadeOutStartTime = 0.5f;
	emitDesc.fadeOut = true;
	emitDesc.particleAcceleration = XMFLOAT3(0, -15.0f, 0);
	emitDesc.emissionStartRadius = 0.01f;
	emitDesc.emissionEndRadius = 3.0f;
	emitDesc.emissionRate = 3.0;
	emitDesc.maxParticles = 100;
	emitDesc.particleInitMinSpeed = 0.001f;
	emitDesc.particleInitMaxSpeed = 0.003f;
	emitDesc.particleMinLifetime = 3.0f;
	emitDesc.particleMaxLifetime = 5.0f;
	emitDesc.particleInitMinScale = 0.03f;
	emitDesc.particleInitMaxScale = 0.05f;
	emitDesc.fadeOutStartTime = 0.5f;
	emitDesc.fadeOut = true;
	emitDesc.particleAcceleration = XMFLOAT3(0, -8.0f, 0);

	new GPUParticleEmitter(emitDesc);

	emitDesc.emissionRate = 0.5;
	emitDesc.emitterPosition.x = emitDesc.emitterPosition.x + 1.0f;
	new GPUParticleEmitter(emitDesc);

	emitDesc.emissionRate = 1.0;
	emitDesc.emitterPosition.x = emitDesc.emitterPosition.x + 1.2f;
	new GPUParticleEmitter(emitDesc);


	Entity* candle = EESceneLoader->SceneEntitiesMap["candle"];
	Entity* candle2 = EESceneLoader->SceneEntitiesMap["candle (1)"];
	Entity* fireplace = EESceneLoader->SceneEntitiesMap["fireplace"];

	//CANDLE FLAME
	emitDesc = {};
	emitDesc.emitterDirection = Y_AXIS;
	emitDesc.emitterPosition = candle->GetTransform().GetPosition();
	emitDesc.emitterPosition.y = emitDesc.emitterPosition.y + 0.8f;
	emitDesc.maxParticles = 100;
	emitDesc.emissionRate = 3.0f;

	ParticleTexture partTex[2] = {
		//{EESceneLoader->texture2DMap["smoke1"], 1.0f},
		{EESceneLoader->Texture2DMap["smoke2"], 1.0f, 0.1f},
		{EESceneLoader->Texture2DMap["smoke3"], 1.0f, 0.2f},
	};
	emitDesc.textures = partTex;
	emitDesc.textureCount = 2;

	emitDesc.emissionStartRadius = 0.03f;
	emitDesc.emissionEndRadius = 0.07f;
	emitDesc.particleInitMinSpeed = 0.05f;
	emitDesc.particleInitMaxSpeed = 0.1f;
	emitDesc.particleMinLifetime = 2.0f;
	emitDesc.particleMaxLifetime = 5.0f;
	emitDesc.particleInitMinScale = 0.2f;
	emitDesc.particleInitMaxScale = 0.3f;
	emitDesc.particleInitMinAngularVelocity = -0.05f;
	emitDesc.particleInitMaxAngularVelocity = 0.05f;

	emitDesc.particleAcceleration = XMFLOAT3(0.0f, 0.0f, -0.0001f);

	emitDesc.fadeOut = true;
	emitDesc.fadeIn = true;
	emitDesc.fadeOutStartTime = 2.0f;
	emitDesc.fadeInEndTime = 0.75f;

	GPUParticleEmitter* gpuEmitter = new GPUParticleEmitter(emitDesc);
	gpuEmitter->SetBlendingEnabled(true);

	emitDesc.emitterPosition = candle2->GetTransform().GetPosition();
	emitDesc.emitterPosition.y = emitDesc.emitterPosition.y + 0.8f;
	gpuEmitter = new GPUParticleEmitter(emitDesc);

	emitDesc.emitterPosition = fireplace->GetTransform().GetPosition();
	emitDesc.emitterPosition.y = emitDesc.emitterPosition.y + 1.0f;
	emitDesc.particleMinLifetime = 5.0f;
	emitDesc.particleMaxLifetime = 10.0f;
	emitDesc.emissionRate = 5.0f;
	emitDesc.particleInitMinScale = 1.0f;
	emitDesc.particleInitMaxScale = 1.2f;
	emitDesc.emissionStartRadius = 0.75f;
	emitDesc.emissionEndRadius = 0.75f;
	gpuEmitter = new GPUParticleEmitter(emitDesc);

	emitDesc.emitterPosition = candle->GetTransform().GetPosition();
	emitDesc.emitterPosition.y = emitDesc.emitterPosition.y + 0.8f;
	emitDesc.fadeOutStartTime = -1.0f;
	emitDesc.fadeInEndTime = 0.05f;
	emitDesc.emissionStartRadius = 0.03f;
	emitDesc.emissionEndRadius = 0.07f;
	ParticleTexture partTexFire[3] = {
		{EESceneLoader->Texture2DMap["fire2"], 3.0f, 0.3f},
		{EESceneLoader->Texture2DMap["fire3"], 1.0f, 0.2f},
		{EESceneLoader->Texture2DMap["fire4"], 1.0f, 0.3f},
	};
	//partTex[0] = { EESceneLoader->texture2DMap["fire"], 1.0f, 0.1f };
	emitDesc.textures = partTexFire;
	emitDesc.textureCount = 3;
	emitDesc.maxParticles = 100;
	emitDesc.emissionRate = 10.0f;
	emitDesc.particleInitMinSpeed = 0.1f;
	emitDesc.particleInitMaxSpeed = 0.2f;
	emitDesc.particleMinLifetime = 0.5f;
	emitDesc.particleMaxLifetime = 0.6f;
	emitDesc.particleInitMinScale = 0.07f;
	emitDesc.particleInitMaxScale = 0.1f;
	emitDesc.particleAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	gpuEmitter = new GPUParticleEmitter(emitDesc);
	gpuEmitter->SetBlendingEnabled(true);

	emitDesc.emitterPosition = candle2->GetTransform().GetPosition();
	emitDesc.emitterPosition.y = emitDesc.emitterPosition.y + 0.8f;
	gpuEmitter = new GPUParticleEmitter(emitDesc);

	emitDesc.emitterPosition = fireplace->GetTransform().GetPosition();
	emitDesc.emitterPosition.y = emitDesc.emitterPosition.y + 1.0f;
	emitDesc.emissionRate = 25.0f;
	emitDesc.particleInitMinScale = 0.2f;
	emitDesc.particleInitMaxScale = 0.5f;
	emitDesc.emissionStartRadius = 0.75f;
	emitDesc.emissionEndRadius = 0.75f;
	gpuEmitter = new GPUParticleEmitter(emitDesc);

	emitDesc = {};
	emitDesc.emitterPosition = XMFLOAT3(0, 4.5f, 0);
	ParticleColor partColors[3] = {
		{XMFLOAT4(1.0f,0,0,0.5f),5.0f},
		{XMFLOAT4(1.0f,0,0,0.5f),5.0f},
		{XMFLOAT4(1.0f,0,0,0.2f),5.0f},
	};
	emitDesc.colors = partColors;
	emitDesc.colorCount = 3;
	emitDesc.maxParticles = 25;
	emitDesc.emissionRate = 5000.0f;
	emitDesc.emissionStartRadius = 0.5f;
	emitDesc.emissionEndRadius = 0.85f;
	//emitDesc.emissionRotation = XMFLOAT3(-XM_PIDIV2,0.0f,0.0f);
	emitDesc.emitterDirection = NEG_Z_AXIS;
	emitDesc.bakeWorldMatOnEmission = true;
	emitDesc.particleInitMinSpeed = 20.0f;
	emitDesc.particleInitMaxSpeed = 30.0f;
	emitDesc.particleMinLifetime = 5.0f;
	emitDesc.particleMaxLifetime = 10.0f;
	//emitDesc.particleInitMinScale = 0.1f;
	//emitDesc.particleInitMaxScale = 0.2f;
	emitDesc.particleInitMinScale = 0.1f;
	emitDesc.particleInitMaxScale = 0.11f;
	emitDesc.particleAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);

	splatter = new CPUParticleEmitter(emitDesc);
	((ParticleEmitter*)splatter)->SetIsActive(false);
	//cpuEmitter->SetBlendingEnabled(true);

	splatter->SetCollisionsEnabled([](void* collision) {
		btPersistentManifold* manifold = (btPersistentManifold*)collision;
		btCollisionObject* obA = (btCollisionObject*)(manifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(manifold->getBody1());

		PhysicsWrapper* wrapperA = (PhysicsWrapper*)obA->getUserPointer();
		PhysicsWrapper* wrapperB = (PhysicsWrapper*)obB->getUserPointer();

		if (wrapperA->type == PHYSICS_WRAPPER_TYPE::PARTICLE && wrapperB->type == PHYSICS_WRAPPER_TYPE::ENTITY ||
			wrapperB->type == PHYSICS_WRAPPER_TYPE::PARTICLE && wrapperA->type == PHYSICS_WRAPPER_TYPE::ENTITY) {

			ParticlePhysicsWrapper* particleWrapper = (wrapperA->type == PHYSICS_WRAPPER_TYPE::PARTICLE) ? (ParticlePhysicsWrapper*)wrapperA->objectPointer : (ParticlePhysicsWrapper*)wrapperB->objectPointer;
			Entity* entity = (wrapperA->type == PHYSICS_WRAPPER_TYPE::ENTITY) ? (Entity*)wrapperA->objectPointer : (Entity*)wrapperB->objectPointer;

			if (entity->HasLayer("decal")) {
				//cout << entity->GetName() << endl;

				Particle* particle = (Particle*)particleWrapper->particle;
				CPUParticleEmitter* emitter = (CPUParticleEmitter*)particleWrapper->emitter;
				ParticleEmitter* emitterBase = (ParticleEmitter*)emitter;

				XMFLOAT3 particleWorld = particle->position;
				XMFLOAT3 normalizedVel = particle->velocity;
				XMStoreFloat3(&normalizedVel, XMVector3Normalize(XMLoadFloat3(&normalizedVel)));

				if (!particle->worldMatBaked) {
					XMVECTOR particlePos = XMLoadFloat3(&particle->position);
					XMMATRIX emitterWorld = XMMatrixTranspose(XMLoadFloat4x4(&emitterBase->GetWorldMatrix()));
					particlePos.m128_f32[3] = 1.0f;
					XMStoreFloat3(&particleWorld, XMVector4Transform(particlePos, emitterWorld));

					XMVECTOR particleVel = XMLoadFloat3(&particle->velocity);
					particleVel.m128_f32[3] = 0.0f;
					XMStoreFloat3(&normalizedVel, XMVector3Normalize(XMVector4Transform(particleVel, emitterWorld)));
				}
				float scaleRand = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				XMFLOAT3 decalScale(1.0f + 4.0f * scaleRand, 1.0f + 4.0f * scaleRand, 5.0f);
				DecalHandler::GetInstance()->GenerateDecal(entity, normalizedVel, particleWorld, decalScale, DecalType(rand() % 8));

				emitter->KillParticle(particleWrapper->particleIndex);
			}
		}
		});
}

void MainMenuManager::Update()
{
	totalTime += deltaTime;

	RECT window;
	RECT client;
	if (GetWindowRect(Config::hWnd, &window)) {
		if (GetClientRect(Config::hWnd, &client)) {
			windowCenter.x = round(client.left + (float)Config::ViewPortWidth / 2.0f);
			windowCenter.y = round(client.top + (float)Config::ViewPortHeight / 2.0f);
			uiCb.windowCenter = windowCenter;

			uiCb.windowWidthRatio = (float)Config::ViewPortWidth / 1600.0f;
			uiCb.windowHeightRatio = (float)Config::ViewPortWidth / 900.0f;
		}
	}

	hearthPtLight->light.Intensity = originalHearth.Intensity + (cos(totalTime * (static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)))) - 0.5f) * (static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) * 0.1f);
	hearthLight->light.Range = originalHearth.Range + (sin(totalTime)) * 3.0f;
	if (hearthLight->light.Intensity < originalHearth.Intensity / 3.0f) hearthLight->light.Intensity = originalHearth.Intensity / 3.0f;
	//menuLight->light.Intensity = originalMenu.Intensity + sin(totalTime / 10.0);

	float scalar = 0.75f;

	if (bloodFired && totalTime - firedTime > 0.1f) {
		((ParticleEmitter*)splatter)->SetIsActive(false);
	}

	if (totalTime >= 5.0f && !tooltipOn && !tooltipClosing) {
		uiCb.skipTooltipTransparency += 1.0f * deltaTime;
		if (uiCb.skipTooltipTransparency > 1.0f) {
			uiCb.skipTooltipTransparency = 1.0f;
			tooltipOn = true;
		}
	}

	if (mouse->OnLMBDown() && !lerpingPaperSide && !lerpingPaperBottom && paperCounter != numPapers) {
		lerpingPaperSide = true;
		originalPaperPos = papers[paperCounter]->GetTransform().GetPosition();
		paperLerpSpd = 0.0f;
		if(uiCb.skipTooltipTransparency == 1.0f)
			tooltipClosing = true;
	}

	if (tooltipClosing) {
		uiCb.skipTooltipTransparency -= 1.0f * deltaTime;
		if (uiCb.skipTooltipTransparency < 0.0f) uiCb.skipTooltipTransparency = 0.0f;
	}

	if ((lerpingPaperSide || lerpingPaperBottom) && paperCounter != numPapers) {
		XMFLOAT3 currentPaperPos = papers[paperCounter]->GetTransform().GetPosition();
		XMVECTOR currentPos = XMLoadFloat3(&currentPaperPos);
		XMFLOAT3 lerpPos;
		XMVECTOR posTo = XMVectorAdd(XMLoadFloat3(&originalPaperPos), (lerpingPaperSide) ? XMLoadFloat3(&paperSidePos) : XMVectorAdd(XMLoadFloat3(&paperBottomPos), XMVectorSet(0, paperYOffset * (numPapers - paperCounter),0,0)));
		XMStoreFloat3(&lerpPos, XMVectorLerp(currentPos, posTo, paperLerpSpd * deltaTime));
		papers[paperCounter]->GetTransform().SetPosition(lerpPos);
		if (XMVector3NearEqual(currentPos, posTo, XMVectorSet(0.1f, 0.1f, 0.1f, 0.1f))) {
			if (lerpingPaperSide) {
				lerpingPaperSide = false;
				lerpingPaperBottom = true;
				paperLerpSpd /= 6.0f;
			}
			else {
				lerpingPaperBottom = false;
				paperCounter++;
			}
		}
		paperLerpSpd += 1.0f * deltaTime;
		if (paperLerpSpd > 3.0f) paperLerpSpd = 3.0f;
	}

	if (Keyboard::GetInstance()->KeyIsPressed(32)) {
		paperCounter = numPapers;
		tooltipClosing = true;
	}
	
	if ((((totalTime - rotTimeStamp) > timeOffsetsRotation[rotCounter]) && nearRot) && rotCounter < 4 && paperCounter == numPapers) {// || (nearRot && rotCounter != 0)  && paperCounter == numPapers
		rotCounter++;
		rotTimeStamp = totalTime;
		rotSpd = 0.0f;
	}

	rotSpd += 0.1f * deltaTime;
	if (rotSpd > 0.4f) rotSpd = 0.4f;

	XMFLOAT3 currentCamRot = cam->GetTransform().GetEulerAnglesRadians();
	XMVECTOR current = XMLoadFloat3(&currentCamRot);
	XMFLOAT3 lerpRot;
	XMVECTOR rotTo = XMLoadFloat3(&rotations[rotCounter]);
	XMStoreFloat3(&lerpRot, XMVectorLerp(current, rotTo, rotSpd * deltaTime));
	nearRot = XMVector3NearEqual(current, rotTo, XMVectorSet(0.1f, 0.1f, 0.1f, 0.1f));
	cam->GetTransform().SetRotationRadians(lerpRot);
	cam->rotation = lerpRot;
	
	if (rotCounter > 3) {
		XMFLOAT3 currentCamPos = cam->GetTransform().GetPosition();
		XMVECTOR currentPos = XMLoadFloat3(&currentCamPos);
		XMFLOAT3 lerpPos;
		XMVECTOR posTo = XMLoadFloat3(&positions[posCounter]);
		XMStoreFloat3(&lerpPos, XMVectorLerp(currentPos, posTo, posSpd * deltaTime));
		cam->GetTransform().SetPosition(lerpPos);
		if (XMVector3NearEqual(currentPos, posTo, XMVectorSet(0.5f, 0.5f, 0.5f, 0.5f)) && posCounter == 1) {
			if (!bloodFired) {
				((ParticleEmitter*)splatter)->SetIsActive(true);
				bloodFired = true;
				firedTime = totalTime;
			}

			menuLight->light.Intensity += scalar * deltaTime;
			if (menuLight->light.Intensity > 0.75f) menuLight->light.Intensity = 0.75f;

			uiCb.transparency += scalar * deltaTime;
			if (uiCb.transparency > 1.0f) uiCb.transparency = 1.0f;
			if (uiCb.transparency == 1.0f && ((totalTime - firedTime) > 2.0f)) {
				uiCb.otherTransparency += scalar * deltaTime;
				if (uiCb.otherTransparency > 1.0f) uiCb.otherTransparency = 1.0f;
			}
		}
		if (XMVector3NearEqual(currentPos, posTo, XMVectorSet(0.3f, 0.3f, 0.3f, 0.3f)) && posCounter < 1) {
			posCounter++;
			posSpd = 0.0f;
		}
		posSpd += 0.1f * deltaTime;

		if (posSpd > 0.7f) posSpd = 0.7f;
	}
}

MainMenuManager::~MainMenuManager()
{
	delete uiCb.spriteBatch;
	delete uiCb.font;
}
