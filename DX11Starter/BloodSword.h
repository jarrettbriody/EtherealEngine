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

	SwordState ss;

	XMFLOAT3 positionLerpTolerance = XMFLOAT3(0.1f, 0.1f, 0.1f);

	float positionLerpScalar = 5.0f;

	XMFLOAT3 lerpPositionFrom = XMFLOAT3(0,0,0);
	XMFLOAT3 lerpPositionTo = XMFLOAT3(0, 0, 0);

	XMFLOAT3 finalLerpPos = XMFLOAT3(0, 0, 0);
	XMFLOAT3 finalLerpDir= XMFLOAT3(0,0,0);

	void Init();

	void Update();

	void IdleState();

	void RaisedState();

	void SlashingState();

	void ResetState();

	bool CheckTransformationsNearEqual();

	void CalcLerp();

	void OnCollision(btCollisionObject* other);

public:
	void StartSlash();

};

