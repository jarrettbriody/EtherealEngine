#pragma once
#include "ScriptManager.h"

enum class SwordState
{
	Slashing, Idle
};

class BloodSword : public ScriptManager
{
	map<string, Entity*>* eMap;

	Camera* cam;

	SwordState ss;
	
	XMFLOAT3 defaultPos;
	XMFLOAT3 startPos;
	XMFLOAT3 endPos;

	float lerpScalar = 25.0f;

	void Init();

	void Update();

	void UpdateSwordTransform();

	void Slash();

	void OnCollision(btCollisionObject* other);

public:
	void StartSlash();

};

