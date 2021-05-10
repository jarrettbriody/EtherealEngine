#include "pch.h"
#include "BloodOrb.h"

void BloodOrb::Init()
{
	fluidCallback = {};

	fluidCallback.vShader = EESceneLoader->VertexShadersMap["Fluid"];
	fluidCallback.pShader = EESceneLoader->PixelShadersMap["Fluid"];
	fluidCallback.prepassVShader = EESceneLoader->VertexShadersMap["FluidPrepass"];
	fluidCallback.prepassPShader = EESceneLoader->PixelShadersMap["FluidPrepass"];
	fluidCallback.fillLineY = -0.25f;
	//fluidCallback.waveHeight = 0.02f;
	fluidCallback.radius = entity->GetTransform().GetScale().x;

	EERenderer->SetRenderObjectCallback(entity, &fluidCallback);
	fluidCallback.active = true;

	ParticleEmitterDescription orbEmitDesc;
	orbEmitDesc.parentName = "Blood_Orb";
	orbEmitDesc.parentWorld = entity->GetTransform().GetWorldMatrixPtr();
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
	Entity* glass = EESceneLoader->SceneEntitiesMap["Blood_Orb_Glass"];
	cam->CalcViewMatrix();
	//cam->CalcWorldMatrix();
	XMFLOAT3 camPos = cam->GetTransform().GetPosition();
	XMFLOAT3 camDir = cam->GetTransform().GetDirectionVector();
	XMFLOAT3 newPos = XMFLOAT3(camPos.x + camDir.x * 1.1f, camPos.y + camDir.y - 0.5f, camPos.z + camDir.z * 1.1f);
	newPos.y = newPos.y + sin(totalTime) * bobMagnitude;
	entity->GetTransform().SetPosition(newPos);
	glass->GetTransform().SetPosition(newPos);

	if (fluidCallback.fillLineY != fillLineMark)
	{
		if (fluidCallback.fillLineY < fillLineMark)fluidCallback.fillLineY += 0.25f * deltaTime;
		if (fluidCallback.fillLineY > fillLineMark)fluidCallback.fillLineY -= 0.25f * deltaTime;
	}

	/*
	if (GetAsyncKeyState(VK_UP))
	{
		fluidCallback.fillLineY += 0.1f * deltaTime;
		if (fluidCallback.fillLineY > fluidCallback.radius) fluidCallback.fillLineY = fluidCallback.radius;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		fluidCallback.fillLineY -= 0.1f * deltaTime;
		if (fluidCallback.fillLineY < -fluidCallback.radius) fluidCallback.fillLineY = -fluidCallback.radius;
	}
	*/

	float modifier = (fluidCallback.radius - pow(abs(fluidCallback.fillLineY), 4.0f));
	if (modifier < 0.01f) modifier = 0.01f;
	else if (modifier > 0.99f) modifier = 0.99f;
	fluidCallback.waveHeight = defaultWaveHeight * modifier;

	fluidCallback.totalTime = totalTime;
	fluidCallback.deltaTime = deltaTime;
	fluidCallback.waveCounter = waveCounter;
	fluidCallback.pos = newPos;

	waveCounter+=5;
	totalTime += deltaTime;
}

void BloodOrb::SetFillLinePercentage(float percentage)
{
	float fillLinePercentage = percentage;

	if (fillLinePercentage > 100)
	{
		fillLinePercentage = 100;
	}
	else if (fillLinePercentage < 0)
	{
		fillLinePercentage = 0;
	}
	// value=min+(max−min)×percentage
	fillLineMark = -fluidCallback.radius + (fluidCallback.radius - (-fluidCallback.radius)) * (fillLinePercentage/100);
	// cout << "Percentage: " << percentage << " fillLineMark: " << fillLineMark << " Max: " << fluidCallback.radius << " Min: " << -fluidCallback.radius << endl;
}
