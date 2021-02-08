#pragma once
#include "ScriptManager.h"
class FPSController : public ScriptManager
{
	map<string, Entity*>* eMap = ScriptManager::sceneEntitiesMap;

	Camera* cam;

	POINT prevMousePos;

	XMFLOAT3 position;
	XMFLOAT3 direction;

	btRigidBody* playerRBody;

	void Init();

	void Update();

	void Move();

	void OnMouseMove(WPARAM buttonState, int x, int y);
};

