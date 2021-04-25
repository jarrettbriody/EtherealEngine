#pragma once
#include "pch.h"
#include "Lights.h"
#include "MemoryAllocator.h"
#include "Config.h"

using namespace std;

class LightHandler
{
private:
	static LightHandler* instance;

	MemoryAllocator* memAlloc = nullptr;

	LightHandler();
	~LightHandler();
	Light* dirLight = nullptr;
	vector<Light*> lightsVec;
	map<string, Light*> lightsMap;
	map<Light*, bool> lightIsDeadMap;
	map<string, vector<Light*>> entityLightMap;
	//vector<LightParentPair>

public:
	static bool SetupInstance();
	static LightHandler* GetInstance();
	static bool DestroyInstance();

	void GarbageCollect();

	void Update();
	void Draw();

	Light* AddLight(string name, Light newLight, string parentEntityName = "", XMFLOAT4X4* parentWorld = nullptr);
	bool RemoveLight(string name);
	bool RemoveLightsByOwner(string name);
	Light* GetLight(string name);
	bool SetLightPairing();
};

