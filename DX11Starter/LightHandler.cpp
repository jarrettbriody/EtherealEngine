#include "pch.h"
#include "LightHandler.h"

LightHandler* LightHandler::instance = nullptr;

LightHandler::LightHandler()
{
	memAlloc = MemoryAllocator::GetInstance();
	lightsVec.reserve(MAX_LIGHTS);
}

LightHandler::~LightHandler()
{
}

bool LightHandler::SetupInstance()
{
	if (instance == nullptr) {
		instance = new LightHandler();
		return true;
	}
	return false;
}

LightHandler* LightHandler::GetInstance()
{
	return instance;
}

bool LightHandler::DestroyInstance()
{
	if (instance != nullptr) {
		delete instance;
		return true;
	}
	return false;
}

void LightHandler::GarbageCollect()
{
	int count = lightsVec.size();
	Light* currentLight = nullptr;
	for (int i = count - 1; i >= 0; i--)
	{
		currentLight = lightsVec[i];
		if (lightIsDeadMap.count(currentLight)) {
			lightsVec.erase(lightsVec.begin() + i);
			lightIsDeadMap.erase(currentLight);
			memAlloc->DeallocateFromPool((unsigned int)MEMORY_POOL::LIGHT_POOL, currentLight, sizeof(Light));
		}
	}
}

Light* LightHandler::AddLight(string name, Light newLight, string parentEntityName, XMFLOAT4X4* parentWorld)
{
	if (lightsVec.size() >= MAX_LIGHTS) return nullptr;

	bool success;
	Light* allocatedLight = (Light*)(memAlloc->AllocateToPool((unsigned int)MEMORY_POOL::LIGHT_POOL, sizeof(Light), success));
	if (!success) return nullptr;

	*allocatedLight = newLight;

	int sameNameCnt = 1;
	string nameToBe = name;
	while (lightsMap.count(nameToBe)) {
		nameToBe = name + " (" + to_string(sameNameCnt) + ")";
		sameNameCnt++;
	}
	lightsMap.insert({ nameToBe, allocatedLight });
	lightsVec.push_back(allocatedLight);

	return allocatedLight;
}

bool LightHandler::RemoveLight(std::string name)
{
	if (!lightsMap.count(name)) return false;
	Light* lightPtr = lightsMap[name];
	lightsMap.erase(name);
	lightIsDeadMap.insert({ lightPtr, true });
	return true;
}

Light* LightHandler::GetLight(string name) {
	if (!lightsMap.count(name)) return nullptr;
	return lightsMap[name];
}
