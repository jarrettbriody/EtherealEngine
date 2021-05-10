#pragma once
#include "Entity.h"
#include "MemoryAllocator.h"
#include "DebugLines.h"

using namespace DirectX;
using namespace std;

enum class DecalType {
	BLOOD1,
	BLOOD2,
	BLOOD3,
	BLOOD4,
	BLOOD5,
	BLOOD6,
	BLOOD7,
	BLOOD8,
};

/*
struct DecalDrawInfo {
	XMFLOAT4X4 world;
	XMFLOAT4X4 invWorld;
	int type;
};
*/

struct Decal {
	XMFLOAT4X4 localTransform;
	XMFLOAT4X4 invLocalTransform;
	int type;
	//TODO add custom color
};

struct DecalBucket {
	Decal decals[MAX_DECALS_PER_ENTITY];
	Entity* owner;
	unsigned int index = 0;
	int counter = 0;
	int count = 0;
	bool alive = true;
};

class DecalHandler
{
private:
	static DecalHandler* instance;
	MemoryAllocator* memAlloc = nullptr;

	DecalHandler();
	~DecalHandler();
public:
	static map<string, DecalBucket*> decalsMap;
	static vector<DecalBucket*> decalsVec;
	//static vector<DecalDrawInfo> decalDrawList;

	static bool SetupInstance();
	static DecalHandler* GetInstance();
	static bool DestroyInstance();

	void GarbageCollect();

	void GenerateDecal(Entity* owner, XMFLOAT3 rayDirection, XMFLOAT3 rayHitPosition, XMFLOAT3 boxScale, DecalType decalType);

	bool DestroyDecalsByOwner(string owner);

	//void UpdateDecals();
};

