#pragma once
#include "ScriptManager.h"
#include <WinUser.h>

class FPSController : public ScriptManager
{
	map<string, Entity*>* eMap;

	Camera* cam;
	POINT prevMousePos = POINT();
	float camRollAngle = 0.0f;
	float camRollSpeed = 5.0f;
	bool rollLeft = false;
	bool rollRight = false;
	const float CAM_ROLL_MIN = -0.05f;
	const float CAM_ROLL_MAX = 0.05f;
	float headbobOffset = 0.0f;
	const float HEADBOB_OFFSET_INTERVAL = 3.5f;
	const float HEADBOB_OFFSET_MIN = 0.0f;
	const float HEADBOB_OFFSET_MAX = 0.5f;
	bool resetHeadbob = false;
	const float NORMAL_FOV = 90.0f;
	const float DASH_FOV = 110.0f;
	float fov = NORMAL_FOV;
	float fovNormalToDashSpeed = 180.0f;
	float fovDashToNormalSpeed = 120.0f;

	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 right;

	// Input variable references to singletons 
	Keyboard* keyboard;
	Mouse* mouse;

	Collider* collider;
	
	btRigidBody* playerRBody;
	btVector3 controllerVelocity;
	btVector3 impulseSumVec;

	float spd = 10.0f;
	float maxSpeed = 25.0f;
	float dampingScalar = 0.09f;

	const unsigned char baseMovementKeys[4] = { 0x57, 0x53, 0x41, 0x44 }; // WASD
	const unsigned char sideMovementKeys[2] = { 0x41, 0x44 }; // WASD

	bool midAir = true; // true if starting character in the air
	int jumpCount = 0;
	float jumpForceScalar = 3.0f;

	int dashCount = 5;
	float dashDampTimer = 0.0f;
	const float DASH_DAMP_TIMER_MAX = 0.3f;
	float dashImpulseScalar = 80.0f;

	btVector3 hookshotPoint;
	float hookshotRangeScalar = 100.0f;

	Entity* leashedEnemy;
	float leashSize = 0.0f; 
	float leashedScalar = 10.0f;

	const float BULLET_TIME_SCALAR = 0.5f;
	const float NORMAL_TIME_SCALAR = 1.0f;
	float bulletTimeRampDown = 0.25f;

	enum class PlayerState
	{
		Intro, Normal, HookshotFlight, HookshotLeash, Paused, Death, Victory
	};
	PlayerState ps;

	void Init();

	void Update();
	
	void CheckAllAbilities();

	void CheckBloodIcicle();
	
	void CheckBulletTime();

	void CheckHookshot();

	void HookshotFlight();

	void HookshotLeash();
	
	void Move();

	void GroundCheck();

	void UpdateHeadbob();
	
	btVector3 JumpForceFromInput();

	btVector3 DashImpulseFromInput();

	void DampForces();

	void MouseLook();

	void OnCollision(btCollisionObject* other);
};

