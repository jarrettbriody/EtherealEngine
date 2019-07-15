#pragma once
//#include "Vertex.h"
#include "Mesh.h"

using namespace DirectX;

class RigidBody
{
private:
	XMFLOAT3 minLocal;
	XMFLOAT3 maxLocal;
	XMFLOAT3 minGlobal;
	XMFLOAT3 maxGlobal;
	XMFLOAT3 centerLocal;
	XMFLOAT3 centerGlobal;
	XMFLOAT3 halfWidth;
	float radius;

	void Init();
public:
	RigidBody(vector<XMFLOAT3> vertices);
	~RigidBody();
};

