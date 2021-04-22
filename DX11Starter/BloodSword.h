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
	XMFLOAT3 directionLerpTolerance = XMFLOAT3(0.25f, 0.25f, 0.25f);

	float positionLerpScalar = 2.0f;
	float directionLerpScalar = 2.0f;

	XMFLOAT3 lerpPositionFrom = XMFLOAT3(0,0,0);
	XMFLOAT3 lerpPositionTo = XMFLOAT3(0, 0, 0);
	XMFLOAT3 lerpDirectionFrom = XMFLOAT3(0, 0, 0);
	XMFLOAT3 lerpDirectionTo = XMFLOAT3(0, 0, 0);

	XMFLOAT3 finalLerpPos = XMFLOAT3(0, 0, 0);
	XMFLOAT3 finalLerpDir= XMFLOAT3(0,0,0);

	std::vector<XMFLOAT3> slashPoints;

	void Init();

	void Update();

	void IdleState();

	void RaisedState();

	void SlashingState();

	void ResetState();

	bool CheckTransformationsNearEqual(bool checkPos, bool checkDir);

	void CalcLerp();

	void OnCollision(btCollisionObject* other);

public:
	void StartSlash();

};

