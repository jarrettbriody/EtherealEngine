#pragma once
#include "Mesh.h"
#include "Utility.h"
//#include "EtherealEngine.h"
#include "DebugLines.h"

using namespace DirectX;

class Collider
{
protected:
	XMFLOAT3 collisionProjVecs[3];
	XMFLOAT3 colliderCorners[8];
	bool collisionsEnabled = true;

private:
	XMFLOAT4X4 worldMatrix;

	XMFLOAT3 minLocal = ZERO_VECTOR3;
	XMFLOAT3 maxLocal = ZERO_VECTOR3;
	XMFLOAT3 minGlobal = ZERO_VECTOR3;
	XMFLOAT3 maxGlobal = ZERO_VECTOR3;
	XMFLOAT3 centerLocal = ZERO_VECTOR3;
	XMFLOAT3 centerGlobal = ZERO_VECTOR3;
	XMFLOAT3 halfWidth = ZERO_VECTOR3;
	XMFLOAT3 span = ZERO_VECTOR3;
	float radius = 0.0f;

	bool debugLinesEnabled = true;
	DebugLines* debugLines = nullptr;

public:
	Collider(vector<XMFLOAT3> vertices);
	~Collider();
	void SetWorldMatrix(XMFLOAT4X4 worldMat);
	unsigned int CheckSATCollision(Collider* other);
	void SetDebugLines(bool dl);
	DebugLines* GetDebugLines();
};

