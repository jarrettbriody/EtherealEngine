#pragma once
#include "Mesh.h"
#include "Utility.h"
#include "Transform.h"

using namespace DirectX;

struct ColliderUpdate : Utility::Callback {
	void* collider;
	void Call();
};

class Collider
{
protected:
	Transform transform;

	XMFLOAT3 collisionProjVecs[3];
	XMFLOAT3 colliderCorners[8];
	XMFLOAT3 untransformedColliderCorners[8];
	XMFLOAT3 pivotShiftedColliderCorners[8];
	bool collisionsEnabled = true;

private:
	string name;
	Mesh* mesh;

	ColliderUpdate callback;

	XMFLOAT3 minLocal = ZERO_VECTOR3;
	XMFLOAT3 maxLocal = ZERO_VECTOR3;
	XMFLOAT3 minGlobal = ZERO_VECTOR3;
	XMFLOAT3 maxGlobal = ZERO_VECTOR3;
	XMFLOAT3 centerLocal = ZERO_VECTOR3;
	XMFLOAT3 centerGlobal = ZERO_VECTOR3;
	XMFLOAT3 halfWidth = ZERO_VECTOR3;
	XMFLOAT3 halfWidthGlobal = ZERO_VECTOR3;
	XMFLOAT3 span = ZERO_VECTOR3;
	float radius = 0.0f;

public:
	bool debugLinesEnabled = false;
	Collider();
	Collider(Mesh* m, vector<XMFLOAT3> vertices);
	~Collider();
	void Update();
	unsigned int CheckSATCollision(Collider* other);
	bool CheckSATCollisionForCorrection(Collider* other, XMFLOAT3& result);
	XMFLOAT3* GetColliderCorners();
	XMFLOAT3* GetUntransformedColliderCorners();
	XMFLOAT3* GetPivotShiftedColliderCorners();
	XMFLOAT3 GetSpan();
	XMFLOAT3 GetHalfWidth();
	XMFLOAT3 GetHalfWidthGlobal();
	XMFLOAT3 GetCenterLocal();
	XMFLOAT3 GetCenterGlobal();
	string GetName();
	Transform& GetTransform();
	Transform* GetTransformPtr();
};

