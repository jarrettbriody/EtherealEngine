#include "pch.h"
#include "BloodOrb.h"

void BloodOrb::Init()
{
	fluidCallback = {};

	fluidCallback.vShader = EESceneLoader->vertexShadersMap["Fluid"];
	fluidCallback.prepassVShader = EESceneLoader->vertexShadersMap["FluidPrepass"];
	fluidCallback.fillLineY = 0.0f;
	fluidCallback.waveHeight = 0.02f;

	EERenderer->SetRenderObjectCallback(entity, &fluidCallback);
	fluidCallback.active = true;

	ParticleEmitterDescription emitDesc;
	emitDesc.parentName = "Blood_Orb";
	emitDesc.parentWorld = entity->GetWorldMatrixPtr();
	emitDesc.emitterPosition = XMFLOAT3(0, -0.25f, 0);
	emitDesc.emissionStartRadius = 0.75f;
	emitDesc.emissionEndRadius = 0.75f;

	ParticleColor partColors[3] = {
		{XMFLOAT4(0.5f,0,0,1.0f),5.0f},
		{XMFLOAT4(0.25f,0,0,1.0f),5.0f},
		{XMFLOAT4(0.75f,0,0,1.0f),5.0f},
	};
	emitDesc.colors = partColors;
	emitDesc.colorCount = 3;
	emitDesc.maxParticles = 100;
	emitDesc.emissionRate = 10.0f;
	//emitDesc.emissionRotation = XMFLOAT3(-XM_PIDIV2,0.0f,0.0f);
	emitDesc.emitterDirection = Y_AXIS;
	emitDesc.particleInitMinSpeed = 0.5f;
	emitDesc.particleInitMaxSpeed = 1.0f;
	emitDesc.particleMinLifetime = 0.5f;
	emitDesc.particleMaxLifetime = 1.0f;
	//emitDesc.particleInitMinScale = 0.1f;
	//emitDesc.particleInitMaxScale = 0.2f;
	emitDesc.particleInitMinScale = 0.005f;
	emitDesc.particleInitMaxScale = 0.02f;
	emitDesc.particleInitMinAngularVelocity = -1.0f;
	emitDesc.particleInitMaxAngularVelocity = 1.0f;
	//emitDesc.particleAcceleration = XMFLOAT3(0.0f, 0.0f, -20.0f);

	orbEmitter = new GPUParticleEmitter(emitDesc);
}

void BloodOrb::Update()
{
	Camera* cam = EERenderer->GetCamera("main");
	Entity* glass = EESceneLoader->sceneEntitiesMap["Blood_Orb_Glass"];
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

	waveCounter+=5;
	totalTime += deltaTime;
}
