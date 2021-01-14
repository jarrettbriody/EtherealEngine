#pragma once

#include <d3d11.h>
#include "Vertex.h"
#include <DirectXMath.h>
#include <vector>
#include <fstream>
#include <regex>

using namespace std;

class Mesh
{
protected:
	static vector<string> mtlPaths;
	vector<DirectX::XMFLOAT3> vertices;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	int indexCount = 0;
	vector<Mesh*> childrenVec;
	Mesh* children = nullptr;
	string mtlPath = "";
	vector<string> materialNameList;
	string meshName = "";
	int childCount = 0;
public:
	Mesh();
	Mesh(Vertex* vertexObjects, int vertexCount, unsigned int* indices, int indexCnt, ID3D11Device* device, string meshN, string matName = "DEFAULT_MATERIAL");
	Mesh(string meshN, char* objFile, ID3D11Device* device, bool* success = nullptr);
	~Mesh();
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();
	void CreateBuffers(Vertex* vertexObjects, int vertexCount, unsigned int* indices, int indexCnt, ID3D11Device* device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	vector<string> GetMaterialNameList();
	string GetFirstMaterialName();
	void AddMaterialName(string nm);
	bool HasChildren();
	Mesh* GetChildren();
	int GetChildCount();
	string GetMTLPath();
	static vector<string> GetMTLPaths();
	void SetVertices(vector<DirectX::XMFLOAT3> verts);
	vector<DirectX::XMFLOAT3> GetVertices();
};

