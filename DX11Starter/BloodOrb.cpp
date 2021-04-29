#include "pch.h"
#include "BloodOrb.h"

void BloodOrb::Init()
{
	fluidCallback = {};

	fluidCallback.vShader = EESceneLoader->vertexShadersMap["Fluid"];
	fluidCallback.pShader = EESceneLoader->pixelShadersMap["Fluid"];
	fluidCallback.prepassVShader = EESceneLoader->vertexShadersMap["FluidPrepass"];
	fluidCallback.prepassPShader = EESceneLoader->pixelShadersMap["FluidPrepass"];
	fluidCallback.fillLineY = 0.0f;
	fluidCallback.waveHeight = 0.02f;

	EERenderer->SetRenderObjectCallback(entity, &fluidCallback);
	fluidCallback.active = true;

	ParticleEmitterDescription orbEmitDesc;
	orbEmitDesc.parentName = "Blood_Orb";
	orbEmitDesc.parentWorld = entity->GetWorldMatrixPtr();
	orbEmitDesc.emitterPosition = XMFLOAT3(0, -0.25f, 0);
	orbEmitDesc.emissionStartRadius = 0.75f;
	orbEmitDesc.emissionEndRadius = 0.75f;

	ParticleColor partColors[3] = {
		{XMFLOAT4(0.5f,0,0,1.0f),5.0f},
		{XMFLOAT4(0.25f,0,0,1.0f),5.0f},
		{XMFLOAT4(0.75f,0,0,1.0f),5.0f},
	};
	orbEmitDesc.colors = partColors;
	orbEmitDesc.colorCount = 3;
	orbEmitDesc.maxParticles = 100;
	orbEmitDesc.emissionRate = 15.0f;
	//emitDesc.emissionRotation = XMFLOAT3(-XM_PIDIV2,0.0f,0.0f);
	orbEmitDesc.emitterDirection = Y_AXIS;
	orbEmitDesc.particleInitMinSpeed = 0.5f;
	orbEmitDesc.particleInitMaxSpeed = 1.0f;
	orbEmitDesc.particleMinLifetime = 0.5f;
	orbEmitDesc.particleMaxLifetime = 1.0f;
	//emitDesc.particleInitMinScale = 0.1f;
	//emitDesc.particleInitMaxScale = 0.2f;
	orbEmitDesc.particleInitMinScale = 0.005f;
	orbEmitDesc.particleInitMaxScale = 0.01f;
	orbEmitDesc.particleInitMinAngularVelocity = -1.0f;
	orbEmitDesc.particleInitMaxAngularVelocity = 1.0f;
	//emitDesc.particleAcceleration = XMFLOAT3(0.0f, 0.0f, -20.0f);
	orbEmitDesc.fadeIn = true;
	orbEmitDesc.fadeOut = true;
	orbEmitDesc.fadeInEndTime = 0.2f;
	orbEmitDesc.fadeOutStartTime = 0.25f;

	orbEmitter = new GPUParticleEmitter(orbEmitDesc);
	orbEmitter->SetBlendingEnabled(true);
}

void BloodOrb::Update()
{
	Camera* cam = EERenderer->GetCamera("main");
	Entity* glass = EESceneLoader->sceneEntitiesMap["Blood_Orb_Glass"];
	cam->CalcViewMatrix();
	cam->CalcWorldMatrix();
	XMFLOAT3 camPos = cam->position;
	XMFLOAT3 camDir = cam->direction;
	XMFLOAT3 newPos = XMFLOAT3(camPos.x + camDir.x * 1.3f, camPos.y + camDir.y - 0.65f, camPos.z + camDir.z * 1.3f);
	entity->SetPosition(newPos);
	glass->SetPosition(newPos);
	entity->CalcWorldMatrix();
	glass->CalcWorldMatrix();

	fluidCallback.totalTime = totalTime;
	fluidCallback.deltaTime = deltaTime;
	fluidCallback.waveCounter = waveCounter;
	fluidCallback.pos = newPos;

	waveCounter+=5;
	totalTime += deltaTime;
}
