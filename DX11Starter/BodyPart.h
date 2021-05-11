#pragma once
#include "ScriptManager.h"
#include "CPUParticleEmitter.h"
#include "GPUParticleEmitter.h"
class BodyPart : public ScriptManager
{
	void Init();
	void Update();

	CPUParticleEmitter* cpuPSys;
	GPUParticleEmitter* gpuPSys;

	double totalTime = 0.0;

	double deactivate = 5.0;
	double timeout = 10.0;

	bool destroyedPSys = false;
};

