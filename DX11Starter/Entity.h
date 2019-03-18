#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"

class Entity
{
private:
	Mesh* mesh;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;
	Material* material;
public:
	Entity(Mesh* entityMesh, Material* mat);
	~Entity();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetRotation();
	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(float x, float y, float z);
	ID3D11Buffer* GetMeshVertexBuffer();
	ID3D11Buffer* GetMeshIndexBuffer();
	int GetMeshIndexCount();
	void CalcWorldMatrix();
	void PrepareMaterial(DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);
	Material* GetMaterial();
};

