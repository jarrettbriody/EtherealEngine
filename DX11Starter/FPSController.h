#pragma once
#include "ScriptManager.h"
#include "BloodSword.h"
#include "BloodOrb.h"
#include <WinUser.h>
#include "DXCore.h"
#include "ParticleEmitter.h"
#include "CPUParticleEmitter.h"
#include "GPUParticleEmitter.h"
#include "BaseEnemy.h"

struct DashBlurCallback : RendererCallback {
	int blurAmount = 1;
	unsigned int playerToolsMask = Config::EntityLayers["playertools"];
	ID3D11ShaderResourceView* layerMask = nullptr;
	void PrePixelShaderCallback() {
		pShader->SetInt("blurAmount", blurAmount);
		pShader->SetInt("playerToolsMask", playerToolsMask);
		pShader->SetShaderResourceView("LayerMasks", layerMask);
	}
};

//class EnemyTest; // Forward declaration to avoid circular dependency 

enum class PlayerState
{
	Intro, Normal, HookshotThrow, HookshotFlight, HookshotLeash, Paused, Death, Victory
};

class FPSController : public ScriptManager
{
	map<string, Entity*>* eMap;

	DashBlurCallback dashBlurCallback;

	BloodOrb* bloodOrbScript;
	BaseEnemy* enemyScript; 

	// Player Attributes
	float bloodResource = 100.0f; // this is always out of 100 

	// Camera related attributes
	Camera* cam;
	XMFLOAT2 prevMousePos;
	double camRollAngle = 0.0f;
	double camRollSpeed = 0.03f;
	bool rollLeft = false;
	bool rollRight = false;
	const float CAM_ROLL_MIN = -0.05f;
	const float CAM_ROLL_MAX = 0.05f;
	float headbobOffset = 0.0f;
	const float HEADBOB_OFFSET_INTERVAL = 3.0f;
	const float HEADBOB_OFFSET_MIN = 0.0f;
	const float HEADBOB_OFFSET_MAX = 0.5f;
	bool resetHeadbob = false;
	const float NORMAL_FOV = 100.0f;
	const float DASH_FOV = NORMAL_FOV + 20.0f;
	float fov = NORMAL_FOV;
	float fovNormalToDashSpeed = 180.0f;
	float fovDashToNormalSpeed = 120.0f;

	// Calculation fields
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

	float spd = 15.0f;
	float maxSpeed = 40.0f;
	float dampingScalar = 0.09f;

	const unsigned char baseMovementKeys[4] = { 0x57, 0x53, 0x41, 0x44 }; // WASD

	// Jumping
	bool midAir = true; // true if starting character in the air
	int jumpCount = 0;
	float jumpForceScalar = 3.0f;

	// Dashing
	const int MAX_DASHES = 4;
	int dashCount = MAX_DASHES;
	float dashDampTimer = 0.0f;
	const float DASH_DAMP_TIMER_MAX = 0.3f;
	float dashImpulseScalar = 80.0f;
	float dashRegenerationTimer = 0.0f;
	const float DASH_MAX_REGENERATION_TIME = 1.5f;
	std::vector<Entity*> dashRings;

	// Sword
	EntityCreationParameters swordParams;
	Entity* sword; 

	// Blood Icicle
	EntityCreationParameters icicleParams;
	float bloodIcicleScalar = 200.0f;
	float bloodIcicleRecoilScalar = 65.0f;
	float bloodIcicleCooldownTimer = 0.0f;
	const float BLOOD_ICICLE_MAX_COOLDOWN_TIME = 2.0f;
	float bloodIcicleRecoilDampTimer = 0.0f;
	const float BLOOD_ICICLE_RECOIL_DAMP_TIMER_MAX = 0.3f;
	
	// Hookshot
	Entity* bloodOrb;
	EntityCreationParameters hookshotParams;
	Entity* hookshot;
	Entity* hookshotAttachedEntity;
	float hookshotLength;
	float hookshotZScale = 1.0f;
	btVector3 hookshotPoint;
	float hookshotThrowSpeed = 350.0f;
	float hookshotRangeScalar = 100.0f;
	const float EXIT_RANGE = 3.5f;

	// Leash
	Entity* leashedEnemy;
	float leashSize = 0.0f; 
	float leashedScalar = 10.0f;
	float leashCancelScalar = 100.0f;
	float leashJumpCancelScalar = 150.0f;
	float leashJumpCancelDampTimer = 0.0f;
	const float LEASH_JUMP_DAMP_TIMER_MAX = 0.5f;
	float leashPullCooldownTimer = 0;
	const float LEASH_PULL_MAX_COOLDOWN_TIME = 0.15f;

	// Bullet Time
	const float BULLET_TIME_SCALAR = 0.5f;
	const float NORMAL_TIME_SCALAR = 1.0f;
	float bulletTimeRampDown = 0.25f;
	const float BULLET_TIME_RAMP_DOWN_TIMER_MAX = 1.0f;
	float bulletTimeRampDownTimer = BULLET_TIME_RAMP_DOWN_TIMER_MAX;

	//sword rotation
	const float MAX_SWORD_ROT = XMConvertToRadians(3.0f);
	float swordTilt = 0.0f;
	float swordRoll = 0.0f;
	float swordRotationSpeed = 0.5f;
	bool swordRollLeft = false;
	bool swordRollRight = false;
	bool swordRollForwards = false;
	bool swordRollBackwards = false;

	//player hit
	float onHitDampTimer = 0.0f;
	const float ON_HIT_DAMP_TIMER_MAX = 1.0f;

	PlayerState ps;

	void Init();

	void Update();
	
	void CheckAllAbilities();

	void CheckBloodSword();

	void CheckBloodIcicle();
	
	void CheckBulletTime();

	void CheckHookshot();

	void HookshotThrow();

	void HookshotFlight();

	void HookshotLeash();

	void UpdateHookShotTransform();

	void UpdateDashRingsTransforms();

	void UpdateDashRingsActive(bool setActive);

	void Move();

	void GroundCheck();

	void UpdateHeadbob();

	void UpdateSwordSway();
	
	btVector3 JumpForceFromInput();

	btVector3 DashImpulseFromInput();

	void DampForces();

	void MouseLook();

	void OnCollision(btCollisionObject* other);

public: 
	PlayerState GetPlayerState();
	
	Entity* GetLeashedEntity();

	void SetLeashedEntity(Entity* e);

	void ResetHookshotTransform();
};

/*
* For reference
* struct EntityCreationParameters {
	string entityName = "";
	string tagName = "";
	string layerName = "";
	string meshName = "";
	string materialName = "";
	string scriptNames[8];
	unsigned int scriptCount = 0;
	XMFLOAT3 position = ZERO_VECTOR3;
	XMFLOAT3 rotationRadians = ZERO_VECTOR3;
	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	float entityMass = 0.0f;
	bool initRigidBody = true;
	BulletColliderShape bulletColliderShape = BulletColliderShape::BOX;
	bool collisionsEnabled = true;
	bool drawEntity = true;
	bool drawShadow = true;
};
*/