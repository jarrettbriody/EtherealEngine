#pragma once
#include "ScriptManager.h"

class FPSController : public ScriptManager
{
	map<string, Entity*>* eMap;

	Camera* cam;
	float headbobOffset = 0.0f;
	const float HEADBOB_OFFSET_INTERVAL = 0.06f;
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

	bool midAir = true; // true if starting character in the air
	int jumpCount = 0;
	float jumpForceScalar = 3.0f;

	int dashCount = 5;
	float dashDampTimer = 0.0f;
	float dashImpulseScalar = 125.0f;

	float hookshotRangeScalar = 10.0f;

	enum class PlayerState
	{
		Intro, Normal, HookshotThrown, HookshotFlight, HookshotLeash, Paused, Death, Victory
	};
	PlayerState ps;

	void Init();

	void Update();
	
	void CheckAbilities();

	void Hookshot();
	
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

