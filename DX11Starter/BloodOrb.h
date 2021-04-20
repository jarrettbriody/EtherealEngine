#pragma once
#include "ScriptManager.h"
#include "GPUParticleEmitter.h"

using namespace std;

struct FluidCallback : RendererCallback {
	float totalTime = 0.0f;
	int waveCounter = 0;
	float fillLineY = 0.0f;
	float deltaTime = 0.0f;
	float waveHeight = 0.0f;

	void PreVertexShaderCallback() {
		vShader->SetFloat("fillLineY", fillLineY);
		vShader->SetFloat("totalTime", totalTime);
		vShader->SetFloat("deltaTime", deltaTime);
		vShader->SetFloat("waveHeight", waveHeight);
		vShader->SetInt("counter", waveCounter);
	}

	void PrePrepassVertexShaderCallback() {
		prepassVShader->SetFloat("fillLineY", fillLineY);
		prepassVShader->SetFloat("totalTime", totalTime);
		prepassVShader->SetFloat("deltaTime", deltaTime);
		prepassVShader->SetFloat("waveHeight", waveHeight);
		prepassVShader->SetInt("counter", waveCounter);
	}
};

class BloodOrb : public ScriptManager
{
	FluidCallback fluidCallback;
	GPUParticleEmitter* orbEmitter;

	float totalTime = 0.0f;
	int waveCounter = 0;

	void Init();

	void Update();
};

