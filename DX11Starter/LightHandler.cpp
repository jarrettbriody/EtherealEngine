#include "pch.h"
#include "LightHandler.h"

LightHandler* LightHandler::instance = nullptr;
LightShaders LightHandler::defaultShaders;

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

void LightHandler::SetDefaultShaders(LightShaders shaders)
{
	if(shaders.VS != nullptr && shaders.PS != nullptr)
		defaultShaders = shaders;
}

void LightHandler::GarbageCollect()
{
	int count = lightsVec.size();
	LightContainer* currentLight = nullptr;
	for (int i = count - 1; i >= 0; i--)
	{
		currentLight = lightsVec[i];
		if (lightIsDeadMap.count(currentLight)) {
			lightsVec.erase(lightsVec.begin() + i);
			lightIsDeadMap.erase(currentLight);
			memAlloc->DeallocateFromPool((unsigned int)MEMORY_POOL::LIGHT_POOL, currentLight, sizeof(LightContainer));
		}
	}
}

void LightHandler::Update(XMFLOAT4X4 camView)
{
	DrawCount = 0;

	LightContainer* current;
	XMMATRIX localTransform;
	XMMATRIX translation;
	XMMATRIX rotation;
	XMMATRIX scale;
	XMVECTOR dir;
	XMFLOAT3 y = Y_AXIS;
	XMVECTOR up;
	XMVECTOR right;
	XMFLOAT4 botRow(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR trans = XMLoadFloat4(&botRow);
	XMFLOAT3 camPos(camView._14, camView._24, camView._34);
	XMFLOAT3 camDir(camView._31, camView._32, camView._33);
	XMVECTOR camPosCalc = XMLoadFloat3(&camPos);
	XMVECTOR camDirCalc = XMLoadFloat3(&camDir);
	XMVECTOR lightPosCalc;
	XMVECTOR lightToCam;
	float sqDistToLight;
	for (size_t i = 0; i < lightsVec.size(); i++)
	{
		current = lightsVec[i];

		lightPosCalc = XMLoadFloat3(&current->light.Position);
		
		translation = XMMatrixTranslationFromVector(lightPosCalc);

		dir = XMLoadFloat3(&current->light.Direction);
		dir = XMVector3Normalize(dir);
		up = XMLoadFloat3(&y);
		right = XMVector3Cross(up, dir);
		right = XMVector3Normalize(right);
		up = XMVector3Cross(dir, right);
		up = XMVector3Normalize(up);
		rotation = XMMATRIX(right, up, dir, trans);
		//scale = DirectX::XMMatrixScaling(current->light.Scale.x, current->light.Scale.y, current->light.Scale.z);

		//localTransform = scale * rotation * translation;
		localTransform = rotation * translation;

		if (current->parentWorld != nullptr)
			XMStoreFloat4x4(&current->world, XMMatrixTranspose(XMMatrixMultiply(localTransform, XMMatrixTranspose(XMLoadFloat4x4(current->parentWorld)))));
		else 
			XMStoreFloat4x4(&current->world, XMMatrixTranspose(localTransform));

		if (current->light.Type == LIGHT_TYPE_DIR) {
			DrawList[DrawCount] = current->light;
			DrawCount++;
			continue;
		}

		lightToCam = XMVectorSubtract(lightPosCalc, camPosCalc);

		sqDistToLight = XMVector4LengthSq(lightToCam).m128_f32[0];

		if (sqDistToLight > MAX_LIGHT_RENDER_DIST_SQ) continue;

		//sqDistToLight = XMVector3Dot(camDirCalc, lightToCam).m128_f32[0];

		//if (sqDistToLight < 0.0f && sqDistToLight > -current->light.Range) continue;

		DrawList[DrawCount] = current->light;
		DrawCount++;
	}
}

LightContainer* LightHandler::AddLight(LightContainer newLight)
{
	if (lightsVec.size() >= MAX_LIGHTS) return nullptr;

	bool success;
	LightContainer* allocatedLight = (LightContainer*)(memAlloc->AllocateToPool((unsigned int)MEMORY_POOL::LIGHT_POOL, sizeof(LightContainer), success));
	if (!success) return nullptr;

	*allocatedLight = newLight;

	int sameNameCnt = 1;
	string originalName = newLight.lightName.STDStr();
	string nameToBe = originalName;
	while (lightsMap.count(nameToBe)) {
		nameToBe = originalName + " (" + to_string(sameNameCnt) + ")";
		sameNameCnt++;
	}
	lightsMap.insert({ nameToBe, allocatedLight });
	lightsVec.push_back(allocatedLight);

	string parentName = allocatedLight->parentName.STDStr();
	if (parentName != "" && allocatedLight->parentWorld != nullptr) {
		if (!entityLightMap.count(parentName)) entityLightMap.insert({ parentName, map<string,LightContainer*>() });
		entityLightMap[parentName].insert({ nameToBe, allocatedLight });
	}

	if (allocatedLight->light.Type == LIGHT_TYPE_DIR) dirLight = allocatedLight;

	return allocatedLight;
}

bool LightHandler::RemoveLight(std::string name)
{
	if (!lightsMap.count(name)) return false;
	LightContainer* lightPtr = lightsMap[name];
	lightsMap.erase(name);
	lightIsDeadMap.insert({ lightPtr, true });
	for (auto Iter = entityLightMap.begin(); Iter != entityLightMap.end(); ++Iter)
	{
		if (Iter->second.count(name)) {
			Iter->second.erase(name);
			break;
		}
	}
	return true;
}

bool LightHandler::RemoveLightsByOwner(string name)
{
	if(!entityLightMap.count(name)) return false;
	for (auto Iter = entityLightMap[name].begin(); Iter != entityLightMap[name].end(); ++Iter)
	{
		lightIsDeadMap.insert({ Iter->second, true });
		lightsMap.erase(Iter->second->lightName.STDStr());
	}
	entityLightMap.erase(name);
}

LightContainer* LightHandler::GetLight(string name) {
	if (!lightsMap.count(name)) return nullptr;
	return lightsMap[name];
}

map<string, LightContainer*> LightHandler::GetLights(string entityName)
{
	if (!entityLightMap.count(entityName)) return map<string, LightContainer*>();
	return entityLightMap[entityName];
}
