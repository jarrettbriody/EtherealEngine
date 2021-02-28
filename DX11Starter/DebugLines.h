#pragma once
#include "pch.h"
#include "Utility.h"
#include "Config.h"
//#include "EtherealEngine.h"

using namespace DirectX;
using namespace std;

enum DEBUGLINESTYPE {
	CUBE,
	LINE
};

struct DebugLinesVertex {
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

class DebugLines : public btIDebugDraw {

public:
	static vector<DebugLines*> debugLines;
	static map<string, DebugLines*> debugLinesMap;

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	DebugLinesVertex* vertices = nullptr;
	UINT* indices = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
	XMFLOAT3 color;
	XMFLOAT4X4 worldMatrix;
	string entityName;
	int colliderID;
	bool willUpdate;
	bool destroyed = false;
	int m_debugMode;

	DebugLines(string entityName = "UNNAMED", int colliderID = 0, bool willUpdate = true);

	~DebugLines();

	void GenerateCuboidVertexBuffer(XMFLOAT3* verts, int vertCount);

	// https://www.cs.kent.edu/~ruttan/GameEngines/lectures/Bullet_User_Manual Page 16 of Bullet Manual

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;

	void reportErrorWarning(const char* warningString) override;
	
	void draw3dText(const btVector3& location, const char* textString) override;
	
	void setDebugMode(int debugMode) override;
	
	int getDebugMode() const override;

	void Destroy();
};