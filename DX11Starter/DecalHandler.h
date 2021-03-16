#pragma once
#include "Entity.h"

using namespace DirectX;
using namespace std;

enum class DecalType {
	BLOOD1,
};

struct Decal {
	XMFLOAT4X4 localTransform;
	int type;
};

struct DecalBucket {
	Decal decals[MAX_DECALS_PER_ENTITY];
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
	static map<string, DecalBucket> decalsMap;

	static bool SetupInstance();
	static DecalHandler* GetInstance();
	static bool DestroyInstance();

	bool GenerateDecal(Entity* owner, XMFLOAT3 rayDirection, XMFLOAT3 rayHitPosition, XMFLOAT3 boxScale, DecalType decalType);

	bool DestroyDecals(string owner);
};

