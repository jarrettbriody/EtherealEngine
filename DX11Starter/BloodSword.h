#pragma once
#include "ScriptManager.h"

enum class SwordState
{
	Raised, Slashing, Idle, Reset
};

class BloodSword : public ScriptManager
{
	map<string, Entity*>* eMap;

	Camera* cam;

	Keyboard* keyboard;

	SwordState ss;

	XMFLOAT3 positionLerpTolerance = XMFLOAT3(0.1f, 0.1f, 0.1f);
	XMFLOAT3 rotationLerpTolerance = XMFLOAT3(0.1f, 0.1f, 0.1f);

	float positionLerpScalar = 150.0f;
	float rotationLerpScalar = 150.0f;

	XMFLOAT3 lerpPositionFrom = XMFLOAT3(0, 0, 0);
	XMFLOAT3 lerpPositionTo = XMFLOAT3(0, 0, 0);
	XMFLOAT3 lerpRotationFrom = XMFLOAT3(0, 0, 0);
	XMFLOAT3 lerpRotationTo = XMFLOAT3(0, 0, 0);

	XMFLOAT3 finalLerpPos = XMFLOAT3(0, 0, 0);
	XMFLOAT3 finalLerpRot = XMFLOAT3(0, 0, 0);

	std::vector<XMFLOAT3> slashPointsRight;
	std::vector<XMFLOAT3> slashPointsLeft;
	
	std::vector<XMFLOAT3> slashPoints;
	int slashPointsIndex = 0;
	XMFLOAT3 slashRotation;
	XMFLOAT3 raisedRotation;

	/*TODO: Jitter Issue?
	bool posLerpComplete = false;
	bool rotLerpComplete = false;*/

	void Init();

	void Update();

	void IdleState();

	void RaisedState();

	void SlashingState();

	void ResetState();

	bool CheckTransformationsNearEqual(bool checkPos, bool checkRot);

	void CalcLerp();

	void OnCollision(btCollisionObject* other);

public:
	void StartSlash();

};

