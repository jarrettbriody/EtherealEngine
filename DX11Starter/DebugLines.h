#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <iostream>
#include <vector>
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

class DebugLines {

public:
	static vector<DebugLines*> debugLines;
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

	DebugLines(string entityName = "UNNAMED", int colliderID = 0, bool willUpdate = true);

	~DebugLines();

	void GenerateCuboidVertexBuffer(XMFLOAT3* verts, int vertCount);
};