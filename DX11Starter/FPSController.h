#pragma once
#include "ScriptManager.h"
#include <WinUser.h>

class FPSController : public ScriptManager
{
	map<string, Entity*>* eMap;

	Camera* cam;
	float camRollAngle = 0.0f;
	float headbobOffset = 0.0f;
	const float HEADBOB_OFFSET_INTERVAL = 3.5f;
	const float HEADBOB_OFFSET_MIN = 0.0f;
	const float HEADBOB_OFFSET_MAX = 0.5f;
	bool resetHeadbob = false;

	POINT prevMousePos;

	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 right;

	Collider* collider;
	
	btRigidBody* playerRBody;
	btVector3 controllerVelocity;
	btVector3 impulseSumVec;

	float spd = 10.0f;
	float maxSpeed = 25.0f;
	float dampingScalar = 0.09f;

	const unsigned char baseMovementKeys[4] = { 0x57, 0x53, 0x41, 0x44 }; // WASD

	bool midAir = true; // true if starting character in the air
	int jumpCount = 0;
	float jumpForceScalar = 3.0f;

	int dashCount = 5;
	float dashDampTimer = 0.0f;
	float dashImpulseScalar = 125.0f;

	btVector3 hookshotPoint;
	float hookshotRangeScalar = 100.0f;

	enum class PlayerState
	{
		Intro, Normal, HookshotFlight, HookshotLeash, Paused, Death, Victory
	};
	PlayerState ps;

	void Init();

	void Update();
	
	void CheckAbilities();

	void StartHookshot();

	void HookshotFlight();
	
	void Move();

	void GroundCheck();

	void UpdateHeadbob();
	
	btVector3 JumpForceFromInput();

	btVector3 DashImpulseFromInput();

	void DampForces();

	void OnMouseMove(WPARAM buttonState, int x, int y);

	void OnMouseDown(WPARAM buttonState, int x, int y);

	void OnCollision(btCollisionObject* other);
};

