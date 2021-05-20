#pragma once
#include "ScriptManager.h"
#include "CPUParticleEmitter.h"
#include "GPUParticleEmitter.h"
class BodyPart : public ScriptManager
{
	void Init();
	void Update();

	void CheckPoolSpawn();

	CPUParticleEmitter* cpuPSys;
	GPUParticleEmitter* gpuPSys;

	EntityCreationParameters bloodPoolParams;

	double totalTime = 0.0;

	double deactivate = 5.0;
	double timeout = 10.0;
	float killPart = 45.0f;

	bool destroyedPSys = false;

	bool poolSpawned = false;
};

