#pragma once

#include "pch.h"
#include "Vertex.h"
#include "MemoryAllocator.h"
#include "Utility.h"

using namespace std;

class Mesh
{
protected:
	static vector<string> mtlPaths;
	vector<DirectX::XMFLOAT3>* vertices = nullptr;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	Vertex* vertexArray = nullptr;
	unsigned int* indexArray = nullptr;
	int vertexCount = 0;
	int indexCount = 0;
	vector<Mesh*>* childrenVec = nullptr;
	Mesh** children = nullptr;
	string* mtlPath = nullptr;
	vector<string>* materialNameList = nullptr;
	string* meshName = nullptr;
	int childCount = 0;
public:
	Mesh();
	Mesh(Vertex* vertexObjects, int vertexCnt, unsigned int* indices, int indexCnt, ID3D11Device* device, string meshN, string matName = "DEFAULT_MATERIAL");
	Mesh(string meshN, char* objFile, ID3D11Device* device, bool* success = nullptr);
	~Mesh();
	void operator= (const Mesh& m);
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	Vertex* GetVertexArray();
	unsigned int* GetIndexArray();
	int GetVertexCount();
	int GetIndexCount();
	void CreateBuffers(Vertex* vertexObjects, int vertexCnt, unsigned int* indices, int indexCnt, ID3D11Device* device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	vector<string> GetMaterialNameList();
	string GetMaterialName(unsigned int index = 0);
	unsigned int AddMaterialName(string nm);
	bool HasChildren();
	Mesh** GetChildren();
	int GetChildCount();
	string GetMTLPath();
	static vector<string> GetMTLPaths();
	void SetVertices(vector<DirectX::XMFLOAT3> verts);
	vector<DirectX::XMFLOAT3> GetVertices();
	void FreeMemory();
	void ReleaseBuffers();
	void AllocateChildren();
	string GetName();
	void SetName(string name);
};

