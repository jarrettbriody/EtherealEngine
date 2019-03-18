#pragma once

#include <d3d11.h>
#include "Vertex.h"
#include <DirectXMath.h>
#include <vector>
#include <fstream>

class Mesh
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int indexCount = 0;
public:
	Mesh();
	Mesh(Vertex* vertexObjects, int vertexCount, unsigned int* indices, int indexCnt, ID3D11Device* device);
	Mesh(char* objFile, ID3D11Device* device);
	~Mesh();
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();
	void CreateBuffers(Vertex* vertexObjects, int vertexCount, unsigned int* indices, int indexCnt, ID3D11Device* device);
};

