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

	Collider* collider;

	float spd = 1.0f;
	float maxSpeed = 10.0f;

	bool midAir = false;
	int jumpCount = 0;

	void Init();

	void Update();

	void Move();

	void OnMouseMove(WPARAM buttonState, int x, int y);
};

