#pragma once
#include "ScriptManager.h"

class FPSController : public ScriptManager
{
	map<string, Entity*>* eMap;

	Camera* cam;

	POINT prevMousePos;

	XMFLOAT3 position;
	XMFLOAT3 direction;

	Collider* collider;
	
	btRigidBody* playerRBody;
	btVector3 controllerVelocity;

	float spd = 5.0f;
	float maxSpeed = 20.0f;
	float dampingScalar = 0.09f;

	bool midAir = true; // true if starting character in the air
	int jumpCount = 0;
	float spacingTimer = 0.0f;

	int dashCount = 5;
	float dashScalar = 50.0f;

	enum class PlayerState
	{
		Intro, Normal, HookshotThrown, HookshotFlight, HookshotLeash, Paused, Death, Victory
	};
	PlayerState ps;

	void Init();

	void Update();

	void Move();

	void DampControllerVelocity();

	void OnMouseMove(WPARAM buttonState, int x, int y);

	void OnCollision(btCollisionObject* other);
};

