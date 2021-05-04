#pragma once

#include "pch.h"
#include "Vertex.h"
#include "MemoryAllocator.h"
#include "Utility.h"
#include "EEString.h"

using namespace std;
using namespace DirectX;

class Mesh
{
protected:
	vector<DirectX::XMFLOAT3>* vertices = nullptr;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	int indexCount = 0;
	vector<Mesh*>* childrenVec = nullptr;
	Mesh** children = nullptr;
	vector<string>* materialNameList = nullptr;
	EEString<EESTRING_SIZE> meshName;
	EEString<EESTRING_SIZE> mtlPath;
	int childCount = 0;
	Mesh* centeredMesh = nullptr;
	vector<Vertex>* verts = nullptr;
	vector<unsigned int>* indices = nullptr;
public:
	Mesh();
	Mesh(Vertex* vertexObjects, int vertexCount, unsigned int* indices, int indexCnt, string meshN, string matName = "DEFAULT_MATERIAL");
	Mesh(string meshN, char* objFile, bool* success = nullptr);
	~Mesh();
	void operator= (const Mesh& m);
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();
	void CreateBuffers(Vertex* vertexObjects, int vertexCount, unsigned int* indices, int indexCnt);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	vector<string> GetMaterialNameList();
	string GetMaterialName(unsigned int index = 0);
	unsigned int AddMaterialName(string nm);
	bool HasChildren();
	Mesh** GetChildren();
	int GetChildCount();
	string GetMTLPath();
	void SetVertices(vector<DirectX::XMFLOAT3> verts);
	vector<DirectX::XMFLOAT3> GetVertices();
	void FreeMemory();
	void ReleaseBuffers();
	void AllocateChildren();
	string GetName();
	void GenerateCenteredMesh(XMFLOAT3 offset);
	Mesh* GetCenteredMesh();
};

