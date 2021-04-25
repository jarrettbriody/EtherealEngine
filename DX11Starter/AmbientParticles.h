#pragma once
#include "ScriptManager.h"
#include "GPUParticleEmitter.h"
#include "CPUParticleEmitter.h"

class AmbientParticles : public ScriptManager
{
	GPUParticleEmitter* ambientAsh;

	void Init();
	void Update();
};

