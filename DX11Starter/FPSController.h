#pragma once
#include "ScriptManager.h"
class FPSController : public ScriptManager
{
	map<string, Entity*>* eMap = ScriptManager::sceneEntitiesMap;

	Camera* cam;

	POINT prevMousePos;

	XMFLOAT3 position;
	XMFLOAT3 direction;

	Collider* collider;
	
	btRigidBody* playerRBody;
	btVector3 controllerVelocity;
	float dampingScalar = 0.09f;

	float spd = 5.0f;
	float maxSpeed = 20.0f;

	bool midAir = false;
	int jumpCount = 0;

	void Init();

	void Update();

	void Move();

	void DampControllerVelocity();

	void OnMouseMove(WPARAM buttonState, int x, int y);
};

