#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Material.h"
#include "Collider.h"
#include "PhysicsWrapper.h"
#include "EEString.h"

struct ShadowData {
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix[MAX_SHADOW_CASCADES];
	ID3D11ShaderResourceView* shadowSRV[MAX_SHADOW_CASCADES];
	float nears[MAX_SHADOW_CASCADES];
	float fars[MAX_SHADOW_CASCADES];
	float range[MAX_SHADOW_CASCADES];
	XMFLOAT2 widthHeight[MAX_SHADOW_CASCADES];
	unsigned int cascadeCount = 0;
	XMFLOAT3 sunPos;
	ID3D11SamplerState* shadowSampler = nullptr;
};

struct DepthStencilData {
	ID3D11ShaderResourceView* depthStencilSRV = nullptr;
	ID3D11SamplerState* depthStencilSampler = nullptr;
};

enum class BulletColliderShape {
	BOX,
	CAPSULE,
};

using namespace std;
using namespace DirectX;

class Entity
{
private:
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 invWorldMatrix;
	Mesh* mesh = nullptr;
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 rotationInDegrees;
	XMFLOAT4X4* parentWorld = nullptr;

	XMFLOAT3 direction;
	XMFLOAT3 up;
	XMFLOAT3 right;

	DirectX::XMFLOAT2 repeatTex;
	map<string, Material*>* materialMap = nullptr;
	unsigned int meshMaterialIndex = 0;
	EEString<64> name;
	vector<Entity*>* children = nullptr;
	Entity* parent = nullptr;
	vector<Collider*>* colliders = nullptr;

	bool shadowsEnabled = true;
	ShadowData shadowData;

	DepthStencilData depthStencilData;

	float mass;

	btCompoundShape* compoundShape = nullptr;
	btCollisionShape** collShape = nullptr;
	btRigidBody* rBody = nullptr;

	int colliderCnt = 0;

	PhysicsWrapper pWrap;
public:
	bool destroyed = false;
	bool isCollisionStatic = true;
	bool collisionsEnabled = false;
	bool colliderDebugLinesEnabled = false;
	bool isEmptyObj = false;
	bool renderObject = true;
	EEString<64> tag;
	EEString<64> layer;
	Entity();
	Entity(string entityName);
	Entity(string entityName, Mesh* entityMesh, Material* mat = nullptr);
	~Entity();
	void operator= (const Entity& e);
	void InitRigidBody(BulletColliderShape shape, float entityMass, bool zeroObjects = false);
	void SetWorldMatrix(XMFLOAT4X4 matrix);
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetInverseWorldMatrix();
	XMFLOAT4X4* GetWorldMatrixPtr();
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT3 GetEulerAngles();
	DirectX::XMFLOAT3 GetEulerAnglesDegrees();
	DirectX::XMFLOAT4 GetRotationQuaternion();
	XMFLOAT3 GetDirectionVector();
	XMFLOAT3 GetUpVector();
	XMFLOAT3 GetRightVector();
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 p);
	void SetRigidbodyPosition(btVector3 position, btVector3 orientation);
	void SetScale(float x, float y, float z);
	void SetScale(XMFLOAT3 s);
	void SetRotation(float x, float y, float z);
	void SetRotation(XMFLOAT4 quat);
	void SetRotation(XMFLOAT3 rotRadians);
	void RotateAroundAxis(XMFLOAT3 axis, float scalar);
	void CalcEulerAngles();
	void SetDirectionVector(XMFLOAT3 direction);
	void SetDirectionVectorU(XMFLOAT3 direction, XMFLOAT3 up);
	void SetDirectionVectorR(XMFLOAT3 direction, XMFLOAT3 right);
	void SetDirectionVectorUR(XMFLOAT3 direction, XMFLOAT3 up, XMFLOAT3 right);
	void SetUpVector(XMFLOAT3 up);
	void SetRightVector(XMFLOAT3 right);
	void CalcDirectionVector();
	void SetRepeatTexture(float x, float y);
	void SetShadowData(ShadowData shadowData);
	void SetDepthStencilData(DepthStencilData depthStencilData);
	void SetMeshAndMaterial(Mesh* mesh, Material* mat = nullptr);
	void ToggleShadows(bool toggle);
	void Move(XMFLOAT3 f);
	void Move(float x, float y, float z);
	ID3D11Buffer* GetMeshVertexBuffer(int childIndex = -1);
	ID3D11Buffer* GetMeshIndexBuffer(int childIndex = -1);
	int GetMeshIndexCount(int childIndex = -1);
	string GetMeshMaterialName(int childIndex = -1);
	void CalcWorldMatrix();
	void SetParentWorldMatrix(XMFLOAT4X4* parentWorld);
	XMMATRIX CalcWorldToModelMatrix();
	void PrepareMaterialForDraw(string n, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);
	Material* GetMaterial(string n);
	bool MeshHasChildren();
	int GetMeshChildCount();
	Mesh* GetMesh();
	vector<string> GetMaterialNameList();
	void AddMaterial(Material* mat, bool addToMesh = false);
	string GetName();
	void AddChildEntity(Entity* child, XMFLOAT4X4 childWorldMatrix);
	void AddAutoBoxCollider();
	bool CheckSATCollision(Entity* other);
	bool CheckSATCollisionAndCorrect(Entity* other);
	vector<Collider*> GetColliders();
	btRigidBody* GetRBody();
	Collider* GetCollider(int index = 0);
	btCollisionShape* GetBTCollisionShape(int index);
	btCompoundShape* GetBTCompoundShape(int index);
	float GetMass();
	void RemoveFromPhysicsSimulation();
	void EmptyEntity();
	void Destroy();
	void FreeMemory();
};

