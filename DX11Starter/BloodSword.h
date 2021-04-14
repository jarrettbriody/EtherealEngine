#pragma once
#include "ScriptManager.h"

enum class SwordState
{
	SetTrajectory, Slashing, Reset, Idle
};

class BloodSword : public ScriptManager
{
	map<string, Entity*>* eMap;

	SwordState ss;
	
	XMFLOAT3 defaultPos;
	XMFLOAT3 startPos;
	XMFLOAT3 endPos;

	void Init();

	void Update();


	void SetLerpPositions();

	void Slash();

	void ResetSword();

	void OnCollision(btCollisionObject* other);

public:
	void StartSlash();

};

