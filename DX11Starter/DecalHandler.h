#pragma once
#include "Entity.h"
#include "MemoryAllocator.h"
#include "DebugLines.h"

using namespace DirectX;
using namespace std;

enum class DecalType {
	BLOOD1,
};

struct Decal {
	XMFLOAT4X4 localTransform;
	XMFLOAT4X4 invLocalTransform;
	int type;
};

struct DecalBucket {
	Decal decals[MAX_DECALS_PER_ENTITY];
	Entity* owner;
	unsigned int index = 0;
	int counter = 0;
	int count = 0;
};

class DecalHandler
{
private:
	static DecalHandler* instance;
	DecalHandler();
	~DecalHandler();
public:
	static map<string, DecalBucket*> decalsMap;
	static vector<DecalBucket*> decalsVec;

	static bool SetupInstance();
	static DecalHandler* GetInstance();
	static bool DestroyInstance();

	void GenerateDecal(Entity* owner, XMFLOAT3 rayDirection, XMFLOAT3 rayHitPosition, XMFLOAT3 boxScale, DecalType decalType);

	bool DestroyDecals(string owner);
};

