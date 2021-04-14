#pragma once
#include "ScriptManager.h"

using namespace std;

struct FluidCallback : RendererCallback {
	float totalTime;
	int waveCounter;
	float fillLineY;
	float deltaTime;
	float waveHeight;

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

class TestScript : public ScriptManager
{
	map<string, Entity*>* eMap = ScriptManager::sceneEntitiesMap;

	string test = "Scope";

	FluidCallback fluidCallback;

	float totalTime = 0.0f;
	int waveCounter = 0;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);
};

