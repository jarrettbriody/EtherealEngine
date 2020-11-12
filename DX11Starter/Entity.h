#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <map>
#include "Mesh.h"
#include "Material.h"
#include "Collider.h"
#include "btBulletDynamicsCommon.h"
#include <iostream>

struct ShadowData {
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
	ID3D11ShaderResourceView* shadowSRV = nullptr;
	ID3D11SamplerState* shadowSampler = nullptr;
};

using namespace std;
//using namespace DirectX;

class Entity
{
private:
	Mesh* mesh;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 rotationInDegrees;
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMFLOAT2 repeatTex;
	map<string, Material*> materialMap;
	string name;
	vector<Entity*> children;
	Entity* parent = nullptr;
	vector<Collider*> colliders;

	bool shadowsEnabled = true;
	ShadowData shadowData;

	float isStatic;

	btCollisionShape* collShape;
	btRigidBody* rBody = nullptr;
	btDiscreteDynamicsWorld* dynamicsWorld;
public:
	bool destroyed = false;
	bool isCollisionStatic = true;
	bool collisionsEnabled = true;
	bool colliderDebugLinesEnabled = true;
	Entity(string entityName, Mesh* entityMesh, Material* mat = nullptr);
	~Entity();
	void InitRigidBody(btDiscreteDynamicsWorld* dw);
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetEulerAngles();
	DirectX::XMFLOAT3 GetEulerAnglesDegrees();
	DirectX::XMFLOAT4 GetRotationQuaternion();
	void SetPosition(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetRotation(XMFLOAT4 quat);
	void RotateAroundAxis(XMFLOAT3 axis, float scalar);
	void CalcEulerAngles();
	void SetRepeatTexture(float x, float y);
	void SetShadowData(ShadowData shadowData);
	void ToggleShadows(bool toggle);
	void Move(XMFLOAT3 f);
	void Move(float x, float y, float z);
	ID3D11Buffer* GetMeshVertexBuffer(int i = -1);
	ID3D11Buffer* GetMeshIndexBuffer(int i = -1);
	int GetMeshIndexCount(int i = -1);
	string GetMeshMaterialName(int i = -1);
	void CalcWorldMatrix();
	void PrepareMaterialForDraw(string n, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);
	Material* GetMaterial(string n);
	bool MeshHasChildren();
	int GetMeshChildCount();
	vector<string> GetMaterialNameList();
	void AddMaterialNameToMesh(string nm);
	void AddMaterial(Material* mat);
	string GetName();
	void AddChildEntity(Entity* child);
	void AddAutoBoxCollider();
	bool CheckSATCollision(Entity* other);
	bool CheckSATCollisionAndCorrect(Entity* other);
	vector<Collider*> GetColliders();
	btRigidBody* GetRBody();
	Collider* GetCollider(int index = 0);
	void Destroy();
};

