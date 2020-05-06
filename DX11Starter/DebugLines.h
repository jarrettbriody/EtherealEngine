#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <iostream>
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
	static ID3D11Device* device;

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	DebugLinesVertex* vertices = nullptr;
	UINT* indices = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
	XMFLOAT3 color;
	XMFLOAT4X4 worldMatrix;

	DebugLines();

	~DebugLines();

	void GenerateVertexBuffer(XMFLOAT3* verts, int vertCount);
};