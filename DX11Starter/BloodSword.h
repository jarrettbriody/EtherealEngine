#pragma once
#include "ScriptManager.h"

enum class SwordState
{
	Raised, Slashing, Idle
};

class BloodSword : public ScriptManager
{
	map<string, Entity*>* eMap;

	Camera* cam;

	SwordState ss;

	bool negligentTransformationChange;

	XMFLOAT3 positionLerpTolerance = XMFLOAT3(0.25f, 0.25f, 0.25f);
	XMFLOAT3 rotationLerpTolerance = XMFLOAT3(0.25f, 0.25f, 0.25f);
	XMFLOAT3 directionLerpTolerance = XMFLOAT3(0.25f, 0.25f, 0.25f);

	float positionLerpScalar = 5.0f;
	float rotationLerpScalar = 2.0f;
	float directionLerpScalar = 2.0f;

	XMFLOAT3 lerpPositionFrom = XMFLOAT3(0,0,0);
	XMFLOAT3 lerpPositionTo = XMFLOAT3(0, 0, 0);
	XMFLOAT3 lerpRotationFrom = XMFLOAT3(0, 0, 0);
	XMFLOAT3 lerpRotationTo = XMFLOAT3(0, 0, 0);
	XMFLOAT3 lerpDirectionFrom = XMFLOAT3(0, 0, 0);
	XMFLOAT3 lerpDirectionTo = XMFLOAT3(0, 0, 0);

	void Init();

	void Update();

	void IdleState();

	void RaisedState();

	void SlashingState();

	void ResetSword();

	void OnCollision(btCollisionObject* other);

public:
	void StartSlash();

};

