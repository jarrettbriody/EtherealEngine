#pragma once
#include "ScriptManager.h"
#include "GPUParticleEmitter.h"
#include "CPUParticleEmitter.h"
#include "LightHandler.h"

struct EnemyOutlineCallback : RendererCallback {
	int sampleSize = 1;
	int outlineLayer = 2;
	void PrePixelShaderCallback() {
		pShader->SetShaderResourceView("LayerMap", Renderer::GetInstance()->depthStencilComponents.entityInfoSRV);
		pShader->SetInt("sampleSize", sampleSize);
		pShader->SetInt("outlineLayer", outlineLayer);
	}
};

class AmbientParticles : public ScriptManager
{
	GPUParticleEmitter* ambientAsh;
	EnemyOutlineCallback callback;

	void Init();
	void Update();
};

