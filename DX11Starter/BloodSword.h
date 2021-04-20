#pragma once
#include "ScriptManager.h"

enum class SwordState
{
	Slashing, Reset, Idle
};

class BloodSword : public ScriptManager
{
	map<string, Entity*>* eMap;

	Camera* cam;

	SwordState ss;
	
	XMFLOAT3 lerpTolerance = XMFLOAT3(0.25f, 0.25f, 0.25f);

	XMFLOAT3 defaultPos;
	XMFLOAT3 startPos;
	XMFLOAT3 endPos;

	float lerpScalar = 10.0f;

	void Init();

	void Update();

	void UpdateSwordTransform();

	void Slash();

	void ResetSword();

	void OnCollision(btCollisionObject* other);

public:
	void StartSlash();

};

