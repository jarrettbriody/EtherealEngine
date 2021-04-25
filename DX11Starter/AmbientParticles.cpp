#include "pch.h"
#include "AmbientParticles.h"

void AmbientParticles::Init()
{
	//------------------------AMBIENT ASH------------------------------------
	ParticleEmitterDescription emitDesc;
	emitDesc.emitterPosition = XMFLOAT3(0.0f, 10.0f, 0.0f);
	emitDesc.parentName = "Blood_Orb";
	emitDesc.parentWorld = EESceneLoader->sceneEntitiesMap["Blood_Orb"]->GetWorldMatrixPtr();
	emitDesc.emitterDirection = NEG_Y_AXIS;
	emitDesc.colorCount = 3;
	ParticleColor particleColors[3] = {
		{XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f), 0.5f},
		{XMFLOAT4(0.2f, 0.2f, 0.2f, 0.75f), 1.0f},
		{XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 1.0f},
	};
	emitDesc.colors = particleColors;
	emitDesc.bakeWorldMatOnEmission = true;
	emitDesc.emissionStartRadius = 100.0f;
	emitDesc.emissionEndRadius = 100.0f;
	emitDesc.emissionRate = 500.0f;
	emitDesc.maxParticles = 500000;
	emitDesc.particleInitMinSpeed = 1.0f;
	emitDesc.particleInitMaxSpeed = 3.0f;
	emitDesc.particleMinLifetime = 10.0f;
	emitDesc.particleMaxLifetime = 15.0f;
	emitDesc.particleInitMinScale = 0.01f;
	emitDesc.particleInitMaxScale = 0.015f;
	emitDesc.fadeInEndTime = 0.1f;
	emitDesc.fadeIn = true;
	emitDesc.fadeOutStartTime = 1.0f;
	emitDesc.fadeOut = true;
	emitDesc.particleAcceleration = XMFLOAT3(-0.1f, -0.0005f, 0.2f);

	ambientAsh = new GPUParticleEmitter(emitDesc);
	//----------------------------------------------------------------------------

	//-----------------------FIRE-------------------------------------------------
	emitDesc = {};
	emitDesc.emitterDirection = Y_AXIS;
	emitDesc.emitterPosition = XMFLOAT3(-300, -6.5f, 365);
	emitDesc.maxParticles = 3000;
	emitDesc.emissionRate = 30.0f;

	ParticleTexture partTex[2] = {
		//{EESceneLoader->texture2DMap["smoke1"], 1.0f},
		{EESceneLoader->texture2DMap["smoke2"], 1.0f, 0.2f},
		{EESceneLoader->texture2DMap["smoke3"], 1.0f, 0.3f},
	};
	emitDesc.textures = partTex;
	emitDesc.textureCount = 2;

	emitDesc.emissionStartRadius = 40.0f;
	emitDesc.emissionEndRadius = 40.0f;
	emitDesc.particleInitMinSpeed = 0.5f;
	emitDesc.particleInitMaxSpeed = 1.0f;
	emitDesc.particleMinLifetime = 5.0f;
	emitDesc.particleMaxLifetime = 10.0f;
	emitDesc.particleInitMinScale = 5.0f;
	emitDesc.particleInitMaxScale = 7.0f;
	emitDesc.particleInitMinAngularVelocity = -0.05f;
	emitDesc.particleInitMaxAngularVelocity = 0.05f;

	emitDesc.particleAcceleration = XMFLOAT3(0.0f, 0.0f, -0.0001f);

	emitDesc.fadeOut = true;
	emitDesc.fadeIn = true;
	emitDesc.fadeOutStartTime = 2.0f;
	emitDesc.fadeInEndTime = 0.5f;

	GPUParticleEmitter* gpuEmitter = new GPUParticleEmitter(emitDesc);
	gpuEmitter->SetBlendingEnabled(true);

	emitDesc.emitterPosition = XMFLOAT3(-300, -6.5f, 360);
	emitDesc.emissionStartRadius = 35.0f;
	emitDesc.emissionEndRadius = 35.0f;
	emitDesc.fadeOutStartTime = -1.0f;
	emitDesc.fadeInEndTime = 0.05f;
	ParticleTexture partTexFire[3] = {
		{EESceneLoader->texture2DMap["fire2"], 3.0f, 0.2f},
		{EESceneLoader->texture2DMap["fire3"], 1.0f, 0.1f},
		{EESceneLoader->texture2DMap["fire4"], 1.0f, 0.2f},
	};
	//partTex[0] = { EESceneLoader->texture2DMap["fire"], 1.0f, 0.1f };
	emitDesc.textures = partTexFire;
	emitDesc.textureCount = 3;
	emitDesc.maxParticles = 10000;
	emitDesc.emissionRate = 1000.0f;
	emitDesc.particleInitMinSpeed = 0.1f;
	emitDesc.particleInitMaxSpeed = 0.2f;
	emitDesc.particleMinLifetime = 0.5f;
	emitDesc.particleMaxLifetime = 0.6f;
	emitDesc.particleInitMinScale = 0.5f;
	emitDesc.particleInitMaxScale = 2.0f;
	emitDesc.particleAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	gpuEmitter = new GPUParticleEmitter(emitDesc);
	gpuEmitter->SetBlendingEnabled(true);

	emitDesc.emissionStartRadius = 25.0f;
	emitDesc.emissionEndRadius = 25.0f;

	emitDesc.emitterPosition = XMFLOAT3(-280, -6.5f, 345);
	gpuEmitter = new GPUParticleEmitter(emitDesc);
	gpuEmitter->SetBlendingEnabled(true);

	emitDesc.emitterPosition = XMFLOAT3(-320, -6.5f, 345);
	gpuEmitter = new GPUParticleEmitter(emitDesc);
	gpuEmitter->SetBlendingEnabled(true);

	emitDesc.emitterPosition = XMFLOAT3(-320, -6.5f, 375);
	gpuEmitter = new GPUParticleEmitter(emitDesc);
	gpuEmitter->SetBlendingEnabled(true);

	emitDesc.emitterPosition = XMFLOAT3(-280, -6.5f, 375);
	gpuEmitter = new GPUParticleEmitter(emitDesc);
	gpuEmitter->SetBlendingEnabled(true);

	emitDesc.emitterPosition = XMFLOAT3(-298.8318f, -6.5f, 361.4137f);
	emitDesc.emissionStartRadius = 30.0f;
	emitDesc.emissionEndRadius = 30.0f;
	emitDesc.textureCount = 0;
	emitDesc.colorCount = 3;
	particleColors[0] = { XMFLOAT4(1.0f, 0.313725f, 0, 1.0f), 0.5f };
	particleColors[1] = { XMFLOAT4(1.0f, 0.4823529f, 0, 0.75f), 0.5f };
	particleColors[2] = { XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f), 5.0f };
	emitDesc.colors = particleColors;
	emitDesc.emissionRate = 1000.0f;
	emitDesc.maxParticles = 10000;
	emitDesc.particleInitMinSpeed = 1.0f;
	emitDesc.particleInitMaxSpeed = 2.0f;
	emitDesc.particleMinLifetime = 7.0f;
	emitDesc.particleMaxLifetime = 10.0f;
	emitDesc.particleInitMinScale = 0.02f;
	emitDesc.particleInitMaxScale = 0.03f;
	emitDesc.fadeOutStartTime = -1.0f;
	emitDesc.fadeInEndTime = 0.1f;
	emitDesc.particleAcceleration = XMFLOAT3(-0.2f, 0.2f, 0.5f);

	gpuEmitter = new GPUParticleEmitter(emitDesc);
	//gpuEmitter->SetBlendingEnabled(true);
	//---------------------------------------------------------------------------------------------
}

void AmbientParticles::Update()
{
}
