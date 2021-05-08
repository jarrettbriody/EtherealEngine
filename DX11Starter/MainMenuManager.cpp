#include "pch.h"
#include "MainMenuManager.h"

void MainMenuManager::Init()
{
	cam = EERenderer->GetCamera("main");

	uiCb.spriteBatch = new SpriteBatch(Config::Context);
	uiCb.font = new SpriteFont(Config::Device, L"../../Assets/Fonts/Bloodlust.spritefont");
	EERenderer->SetRenderUICallback(true, &uiCb, 0);

	lights = LightHandler::GetInstance();
	hearthLight = lights->GetLight("hearth");
	menuLight = lights->GetLight("menulight");

	originalHearth = hearthLight->light;
	originalMenu = menuLight->light;

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

	emitDesc.emitterPosition = candle->GetTransform().GetPosition();
	emitDesc.emitterPosition.y = emitDesc.emitterPosition.y + 0.8f;
	emitDesc.fadeOutStartTime = -1.0f;
	emitDesc.fadeInEndTime = 0.05f;
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

	hearthLight->light.Intensity = originalHearth.Intensity + (cos(totalTime * (static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)))) - 0.5f) * (static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) * 0.02f);
	hearthLight->light.Range = originalHearth.Range + (sin(totalTime / 2.0)) * 1.0f;
	if (hearthLight->light.Intensity < originalHearth.Intensity / 3.0f) hearthLight->light.Intensity = originalHearth.Intensity / 3.0f;
	//menuLight->light.Intensity = originalMenu.Intensity + sin(totalTime / 10.0);

	float scalar = 0.75f;

	if (bloodFired && totalTime - firedTime > 0.1f) {
		((ParticleEmitter*)splatter)->SetIsActive(false);
	}

	if (GetAsyncKeyState(VK_UP))
	{
		if (!bloodFired) {
			((ParticleEmitter*)splatter)->SetIsActive(true);
			bloodFired = true;
			firedTime = totalTime;
		}

		menuLight->light.Intensity += scalar * deltaTime;
		if (menuLight->light.Intensity > 0.75f) menuLight->light.Intensity = 0.75f;

		uiCb.transparency += scalar * deltaTime;
		if (uiCb.transparency > 1.0f) uiCb.transparency = 1.0f;
		if(uiCb.transparency == 1.0f && ((totalTime - firedTime) > 2.0f)) {
			uiCb.otherTransparency += scalar * deltaTime;
			if (uiCb.otherTransparency > 1.0f) uiCb.otherTransparency = 1.0f;
		}
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		menuLight->light.Intensity -= scalar * deltaTime;
		if (menuLight->light.Intensity < 0.0001f) menuLight->light.Intensity = 0.0001f; 
		
		uiCb.transparency -= scalar * deltaTime;
		if (uiCb.transparency < 0.0f) uiCb.transparency = 0.0f;
		uiCb.otherTransparency -= scalar * deltaTime;
		if (uiCb.otherTransparency < 0.0f) uiCb.otherTransparency = 0.0f;
	}
	
	if (rotCounter < 5) {
		if ((totalTime - rotTimeStamp) > timeOffsetsRotation[rotCounter]) {
			rotCounter++;
		}
	}
	
}

MainMenuManager::~MainMenuManager()
{
	delete uiCb.spriteBatch;
	delete uiCb.font;
}
