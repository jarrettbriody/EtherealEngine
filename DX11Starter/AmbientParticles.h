#pragma once
#include "ScriptManager.h"
#include "GPUParticleEmitter.h"

class AmbientParticles : public ScriptManager
{
	GPUParticleEmitter* ambientAsh;

	void Init();
	void Update();
};

