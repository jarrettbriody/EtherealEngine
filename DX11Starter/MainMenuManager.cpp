#include "pch.h"
#include "MainMenuManager.h"

void MainMenuManager::Init()
{
	//uiCb.spriteBatch = new SpriteBatch(Config::Context);
	//uiCb.font = new SpriteFont(Config::Device, L"Assets/Fonts/Bloodlust.xnb");

	lights = LightHandler::GetInstance();
	hearthLight = lights->GetLight("hearth");
	menuLight = lights->GetLight("menulight");

	originalHearth = hearthLight->light;
	originalMenu = menuLight->light;

	Entity* bloodsword = EESceneLoader->SceneEntitiesMap["bloodswordlowpoly2"];

	ParticleEmitterDescription emitDesc;
	emitDesc.emitterPosition = bloodsword->GetPosition();
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
	emitDesc.emitterPosition = candle->GetPosition();
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

	emitDesc.emitterPosition = candle2->GetPosition();
	emitDesc.emitterPosition.y = emitDesc.emitterPosition.y + 0.8f;
	gpuEmitter = new GPUParticleEmitter(emitDesc);

	emitDesc.emitterPosition = candle->GetPosition();
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

	emitDesc.emitterPosition = candle2->GetPosition();
	emitDesc.emitterPosition.y = emitDesc.emitterPosition.y + 0.8f;
	gpuEmitter = new GPUParticleEmitter(emitDesc);
}

void MainMenuManager::Update()
{
	totalTime += deltaTime;

	hearthLight->light.Intensity = originalHearth.Intensity + (cos(totalTime * (static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)))) - 0.5f) * (static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) * 0.02f);
	hearthLight->light.Range = originalHearth.Range + (sin(totalTime / 2.0)) * 1.0f;
	if (hearthLight->light.Intensity < originalHearth.Intensity / 3.0f) hearthLight->light.Intensity = originalHearth.Intensity / 3.0f;
	//menuLight->light.Intensity = originalMenu.Intensity + sin(totalTime / 10.0);

	if (GetAsyncKeyState(VK_UP))
	{
		menuLight->light.Intensity += 0.1f * deltaTime;
		if (menuLight->light.Intensity > 0.75f) menuLight->light.Intensity = 0.75f;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		menuLight->light.Intensity -= 0.1f * deltaTime;
		if (menuLight->light.Intensity < 0.0001f) menuLight->light.Intensity = 0.0001f;
	}
}
