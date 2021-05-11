#pragma once
#include "ScriptManager.h"
#include "GPUParticleEmitter.h"
#include "CPUParticleEmitter.h"
#include "LightHandler.h"

struct EnemyOutlineCallback : RendererCallback {
	int sampleSize = 2;
	unsigned int outlineLayerMask = Config::EntityLayers["outline"];
	void PrePixelShaderCallback() {
		pShader->SetShaderResourceView("LayerMap", Renderer::GetInstance()->depthStencilComponents.entityInfoSRV);
		pShader->SetInt("sampleSize", sampleSize);
		pShader->SetInt("outlineLayerMask", outlineLayerMask);
	}
};

class AmbientParticles : public ScriptManager
{
	GPUParticleEmitter* ambientAsh;
	EnemyOutlineCallback callback;

	void Init();
	void Update();
};

