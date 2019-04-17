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
private:
	static vector<string> mtlPaths;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int indexCount = 0;
	vector<Mesh*> children;
	string mtlPath = "";
	vector<string> materialNameList;
	string meshName = "";
	int childCount = 0;
public:
	Mesh();
	Mesh(Vertex* vertexObjects, int vertexCount, unsigned int* indices, int indexCnt, ID3D11Device* device, string meshN, string matName = "DEFAULT_MATERIAL");
	Mesh(string meshN, char* objFile, ID3D11Device* device);
	~Mesh();
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();
	void CreateBuffers(Vertex* vertexObjects, int vertexCount, unsigned int* indices, int indexCnt, ID3D11Device* device);
	vector<string> GetMaterialNameList();
	string GetFirstMaterialName();
	bool HasChildren();
	vector<Mesh*> GetChildren();
	int GetChildCount();
	static vector<string>* GetMtlPaths();
};

