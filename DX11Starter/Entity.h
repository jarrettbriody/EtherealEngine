#pragma once
#include "pch.h"
#include "Mesh.h"
#include "Material.h"
#include "Collider.h"
#include "PhysicsWrapper.h"
#include "EEString.h"
#include "Transform.h"

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
	Transform eTransform;

	Mesh* mesh = nullptr;
	Material* material = nullptr;

	DirectX::XMFLOAT2 repeatTex = ONE_VECTOR2;
	XMFLOAT2 uvOffset = ZERO_VECTOR2;
	map<string, Material*>* materialMap = nullptr;
	unsigned int meshMaterialIndex = 0;
	EEString<EESTRING_SIZE> name = "";
	vector<Entity*>* children = nullptr;
	Entity* parent = nullptr;
	vector<Collider*>* colliders = nullptr;

	bool shadowsEnabled = true;
	ShadowData shadowData = {};

	DepthStencilData depthStencilData = {};

	float mass = 0.0f;

	btCompoundShape* compoundShape = nullptr;
	btCollisionShape** collShape = nullptr;
	btRigidBody* rBody = nullptr;

	int colliderCnt = 0;

	PhysicsWrapper pWrap = {};

	unsigned int tagCount = 0;
	unsigned int layerCount = 0;
	unsigned int layerMask = 0;
	EEString<EESTRING_SIZE> tags[MAX_ENTITY_TAG_COUNT];
	EEString<EESTRING_SIZE> layers[MAX_ENTITY_LAYER_COUNT];
public:
	bool destroyed = false;
	bool isCollisionStatic = true;
	bool collisionsEnabled = false;
	bool colliderDebugLinesEnabled = false;
	bool isEmptyObj = false;
	bool renderObject = true;
	bool hbaoPlusEnabled = true;
	Entity();
	Entity(string entityName);
	Entity(string entityName, Mesh* entityMesh, Material* mat = nullptr);
	~Entity();
	void operator= (const Entity& e);

	Transform& GetTransform();

	void InitRigidBody(BulletColliderShape shape, float entityMass, bool zeroObjects = false);
	void SetRigidbodyPosition(btVector3 position, btVector3 orientation);
	void SetRepeatTexture(float x, float y);
	void SetUVOffset(float x, float y);
	void SetShadowData(ShadowData shadowData);
	void SetDepthStencilData(DepthStencilData depthStencilData);
	void SetMeshAndMaterial(Mesh* mesh, Material* mat = nullptr);
	void ToggleShadows(bool toggle);
	void ToggleHBAOPlus(bool toggle);
	ID3D11Buffer* GetMeshVertexBuffer(int childIndex = -1);
	ID3D11Buffer* GetMeshIndexBuffer(int childIndex = -1);
	int GetMeshIndexCount(int childIndex = -1);
	string GetMeshMaterialName(int childIndex = -1);
	void PrepareMaterialForDraw(string n, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);
	Material* GetMaterial(string n);
	bool MeshHasChildren();
	int GetMeshChildCount();
	Mesh* GetMesh();
	vector<string> GetMaterialNameList();
	void AddMaterial(Material* mat, bool addToMesh = false);
	string GetName();
	bool AddTag(string tag);
	bool RemoveTag(string tag);
	bool HasTag(string tag);
	EEString<EESTRING_SIZE>* GetTags();
	unsigned int GetTagCount();
	bool AddLayer(string layer);
	bool RemoveLayer(string layer);
	bool HasLayer(string layer);
	EEString<EESTRING_SIZE>* GetLayers();
	unsigned int GetLayerCount();
	unsigned int GetLayerMask();
	void AddChild(Entity* child, bool preserveChild = true);
	void AddAutoBoxCollider();
	bool CheckSATCollision(Entity* other);
	bool CheckSATCollisionAndCorrect(Entity* other);
	vector<Collider*> GetColliders();
	btRigidBody* GetRBody();
	Collider* GetCollider(int index = 0);
	btCollisionShape* GetBTCollisionShape(int index);
	btCompoundShape* GetBTCompoundShape(int index);
	void Update();
	float GetMass();
	void RemoveFromPhysicsSimulation();
	void EmptyEntity();
	void Destroy();
	void FreeMemory();
};

