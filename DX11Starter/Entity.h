#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <map>
#include "Mesh.h"
#include "Material.h"

using namespace std;

class Entity
{
private:
	Mesh* mesh;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;
	map<string, Material*> materialMap;
public:
	Entity(Mesh* entityMesh, Material* mat = nullptr);
	~Entity();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetRotation();
	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(float x, float y, float z);
	ID3D11Buffer* GetMeshVertexBuffer(int i = -1);
	ID3D11Buffer* GetMeshIndexBuffer(int i = -1);
	int GetMeshIndexCount(int i = -1);
	string GetMeshMaterialName(int i = -1);
	void CalcWorldMatrix();
	void PrepareMaterial(string n, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);
	Material* GetMaterial(string n);
	bool MeshHasChildren();
	int GetMeshChildCount();
	vector<string> GetMaterialNameList();
	void AddMaterialNameToMesh(string nm);
	void AddMaterial(Material* mat);
};

