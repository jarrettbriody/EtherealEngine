#pragma once
#include "ScriptManager.h"

enum class SwordState
{
	Raised, Slashing, Reset, Idle
};

class BloodSword : public ScriptManager
{
	map<string, Entity*>* eMap;

	Camera* cam;

	SwordState ss;

	XMFLOAT3 positionLerpTolerance = XMFLOAT3(0.25f, 0.25f, 0.25f);
	float rotationLerpTolerance = 0.1f;

	float positionLerpScalar = 10.0f;
	float rotationLerpScalar = 2.0f;

	std::vector<XMFLOAT3> slashLerpPoints;

	void Init();

	void Update();

	void UpdateSwordTransform();

	void Raise();

	void Slash();

	void ResetSword();

	void OnCollision(btCollisionObject* other);

public:
	void StartSlash();

};

