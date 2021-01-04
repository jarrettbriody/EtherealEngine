#pragma once
#include "ScriptManager.h"
class FPSController : public ScriptManager
{
	map<string, Entity*>* eMap = ScriptManager::sceneEntitiesMap;

	Camera* cam;

	POINT prevMousePos;

	XMFLOAT3 position;
	XMFLOAT3 direction;

	void Init();

	void Update();

	void OnMouseMove(WPARAM buttonState, int x, int y);
};

