#pragma once
#include "ScriptManager.h"
#include "GameManager.h"
#include "GPUParticleEmitter.h"
#include "CPUParticleEmitter.h"

#define SWORD_EMITTERS 16

enum class SwordState
{
	Raised, Slashing, Idle, Reset
};

struct SwordWaveCallback : RendererCallback {
	float totalTime;
	float deltaTime;
	float waveHeightX;
	float waveHeightY;
	float waveHeightZ;
	float waveRateX;
	float waveRateY;
	float waveRateZ;

	void PreVertexShaderCallback() {
		vShader->SetFloat("totalTime", totalTime);
		vShader->SetFloat("deltaTime", deltaTime);
		vShader->SetFloat("waveHeightX", waveHeightX);
		vShader->SetFloat("waveHeightY", waveHeightY);
		vShader->SetFloat("waveHeightZ", waveHeightZ);
		vShader->SetFloat("waveRateX", waveRateX);
		vShader->SetFloat("waveRateY", waveRateY);
		vShader->SetFloat("waveRateZ", waveRateZ);
	}

	void PrePrepassVertexShaderCallback() {
		prepassVShader->SetFloat("totalTime", totalTime);
		prepassVShader->SetFloat("deltaTime", deltaTime);
		prepassVShader->SetFloat("waveHeightX", waveHeightX);
		prepassVShader->SetFloat("waveHeightY", waveHeightY);
		prepassVShader->SetFloat("waveHeightZ", waveHeightZ);
		prepassVShader->SetFloat("waveRateX", waveRateX);
		prepassVShader->SetFloat("waveRateY", waveRateY);
		prepassVShader->SetFloat("waveRateZ", waveRateZ);
	}
};

class FPSController; // Forward declaration to avoid circular dependency

class BloodSword : public ScriptManager
{
	map<string, Entity*>* eMap;

	GPUParticleEmitter* emitters[SWORD_EMITTERS];
	XMFLOAT4X4 emitterTransform;

	SwordWaveCallback callback;
	float totalTime = 0.0f;

	GameManager* gameManagerScript;
	FPSController* fpsControllerScript;

	Camera* cam;

	Keyboard* keyboard;

	SwordState ss;

	XMFLOAT3 originalLerpPos = XMFLOAT3(3, -1, 3);
	//XMFLOAT3 originalLerpRot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float bobMagnitude = 0.0005f;

	XMFLOAT3 positionLerpTolerance = XMFLOAT3(0.5f, 0.5f, 0.5f);
	XMFLOAT4 rotationLerpTolerance = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);

	float slashPositionLerpScalar = 15.0f;
	float slashRotationLerpScalar = 15.0f;

	float readyingPositionLerpScalar = 10.0f;
	float readyingRotationLerpScalar = 10.0f;

	bool approachingReset = false;

	XMFLOAT3 lerpPositionFrom = XMFLOAT3(0, 0, 0);
	XMFLOAT3 lerpPositionTo = XMFLOAT3(0, 0, 0);
	XMFLOAT4 lerpRotationFrom;
	XMFLOAT4 lerpRotationTo;

	XMFLOAT3 finalLerpPos = XMFLOAT3(0, 0, 0);
	XMFLOAT4 finalLerpRot;

	std::vector<XMFLOAT3> slashPointsRight;
	std::vector<XMFLOAT3> slashPointsLeft;
	
	std::vector<XMFLOAT3> slashPoints;
	int slashPointsIndex = 0;
	//XMFLOAT3 slashRotation;
	//XMFLOAT3 raisedRotation;

	float viewAngle = 90.0f;
	float viewDistance = 10.0f;

	XMFLOAT4 raisedQuat;
	XMFLOAT4 slashingQuat;
	XMFLOAT4 idleQuat;

	XMFLOAT4 raisedQuatR;
	XMFLOAT4 slashingQuatR;

	XMFLOAT4 raisedQuatL;
	XMFLOAT4 slashingQuatL;

	XMFLOAT3 raisedPos;
	XMFLOAT3 slashPos;

	XMFLOAT3 raisedPosR = XMFLOAT3(5, 2, 3);
	XMFLOAT3 slashPosR = XMFLOAT3(-7, -3, 3);

	XMFLOAT3 raisedPosL = XMFLOAT3(-5, 2, 3);
	XMFLOAT3 slashPosL = XMFLOAT3(7, -3, 3);

	void Init();

	void Update();

	void IdleState();

	void RaisedState();

	void SlashingState();

	void ResetState();

	bool CheckTransformationsNearEqual(bool checkPos, bool checkRot);

	void CalcLerp();

	std::vector<XMFLOAT3> GenerateSlashPoints(XMFLOAT3 startingPos, XMFLOAT3 endingPos, float interval, float maxZ);

	void CheckSwordSlashHit();

	bool EntityInSlashDetectionField(Entity* e);

	void OnCollision(btCollisionObject* other);

public:
	bool animReset = false;
	void StartSlash();
};

