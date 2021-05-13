#pragma once
#include "ScriptManager.h"
#include "GPUParticleEmitter.h"

using namespace std;

struct RenderRingsCallback : RendererCallback {
	void PreDraw() {
		//Config::Context->OMSetDepthStencilState(Renderer::GetInstance()->alwaysDrawDepthStencil, 0);
	}

	void PostDraw() {
		//Config::Context->OMSetDepthStencilState(NULL, 0);
	}
};

struct FluidCallback : RendererCallback {
	float totalTime = 0.0f;
	int waveCounter = 0;
	float fillLineY = 0.0f;
	float deltaTime = 0.0f;
	float waveHeight = 0.0f;
	float radius = 0.0f;
	XMFLOAT3 pos;

	void PreDraw() {
		//Config::Context->OMSetDepthStencilState(Renderer::GetInstance()->alwaysDrawDepthStencil, 0);
	}

	void PreVertexShaderCallback() {
		vShader->SetFloat("fillLineY", fillLineY);
		vShader->SetFloat("totalTime", totalTime);
		vShader->SetFloat("deltaTime", deltaTime);
		vShader->SetFloat("waveHeight", waveHeight);
		//vShader->SetFloat("radius", radius);
		vShader->SetInt("counter", waveCounter);
	}

	void PrePixelShaderCallback() {
		pShader->SetFloat3("orbCenter", pos);
		pShader->SetFloat("radius", radius + 0.002f);
	}

	void PrePrepassVertexShaderCallback() {
		prepassVShader->SetFloat("fillLineY", fillLineY);
		prepassVShader->SetFloat("totalTime", totalTime);
		prepassVShader->SetFloat("deltaTime", deltaTime);
		prepassVShader->SetFloat("waveHeight", waveHeight);
		//prepassVShader->SetFloat("radius", radius);
		prepassVShader->SetInt("counter", waveCounter);
	}
	
	void PrePrepassPixelShaderCallback() {
		prepassPShader->SetFloat3("orbCenter", pos);
		prepassPShader->SetFloat("radius", radius + 0.002f);
	}

	void PostDraw() {
		//Config::Context->OMSetDepthStencilState(NULL, 0);
	}
};

class BloodOrb : public ScriptManager
{
	FluidCallback fluidCallback;
	GPUParticleEmitter* orbEmitter;

	float totalTime = 0.0f;
	int waveCounter = 0;

	float defaultWaveHeight = 0.03f;

	float bobMagnitude = 0.03f;

	float fillLineMark = 0.0f;

	void Init();

	void Update();

public:
	void SetFillLinePercentage(float percentage);
};

