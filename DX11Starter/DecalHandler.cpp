#include "pch.h"
#include "DecalHandler.h"

DecalHandler* DecalHandler::instance = nullptr;
map<string, DecalBucket*> DecalHandler::decalsMap;
vector<DecalBucket*> DecalHandler::decalsVec;
//vector<DecalDrawInfo> DecalHandler::decalDrawList;

DecalHandler::DecalHandler()
{
	//decalDrawList.reserve(Config::InitialDecalVectorSize);
	memAlloc = MemoryAllocator::GetInstance();
}

DecalHandler::~DecalHandler()
{
}

bool DecalHandler::SetupInstance()
{
	if (instance == nullptr) {
		instance = new DecalHandler();
		return true;
	}
	return false;
}

DecalHandler* DecalHandler::GetInstance()
{
	return instance;
}

bool DecalHandler::DestroyInstance()
{
	if (instance != nullptr) {
		delete instance;
		return true;
	}
	return false;
}

void DecalHandler::GarbageCollect()
{
	int count = decalsVec.size();
	DecalBucket* currentBucket = nullptr;
	for (int i = count - 1; i >= 0; i--)
	{
		currentBucket = decalsVec[i];
		if (!currentBucket->alive) {
			decalsVec.erase(decalsVec.begin() + i);
			memAlloc->DeallocateFromPool((unsigned int)MEMORY_POOL::DECAL_POOL, currentBucket, sizeof(DecalBucket));
		}
	}
}

void DecalHandler::GenerateDecal(Entity* owner, XMFLOAT3 rayDirection, XMFLOAT3 rayHitPosition, XMFLOAT3 boxScale, DecalType decalType)
{
	string ownerName = owner->GetName();
	Decal newDecal;
	//newDecal.owner = owner;
	XMFLOAT4 decalCenter;
	//XMVECTOR calculableOwnerCenter = XMLoadFloat3(&owner->GetPosition());
	XMVECTOR calculableDecalCenter = XMLoadFloat3(&rayHitPosition);
	//calculableDecalCenter = XMVectorSubtract(calculableDecalCenter, calculableOwnerCenter);
	XMMATRIX worldToModel = XMMatrixTranspose(XMLoadFloat4x4(&owner->GetTransform().GetInverseWorldMatrix()));
	//calculableDecalCenter = DirectX::XMVector3Transform(calculableDecalCenter, worldToModel);

	XMMATRIX translation = XMMatrixTranslationFromVector(calculableDecalCenter);

	//XMFLOAT3 localRotation = XMFLOAT3(atan2(rayDirection.y, rayDirection.z), atan2(rayDirection.z, rayDirection.x), atan2(rayDirection.y, rayDirection.x));
	//XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(localRotation.x, localRotation.y, localRotation.z);
	//XMMATRIX rotation = XMMatrixRotationAxis(dir, 0.0f);

	XMVECTOR dir = XMLoadFloat3(&rayDirection);
	dir = XMVector3Normalize(dir);
	XMFLOAT3 y = Y_AXIS;
	XMVECTOR up = XMLoadFloat3(&y);
	XMVECTOR right = XMVector3Cross(up,dir);
	right = XMVector3Normalize(right);
	up = XMVector3Cross(dir, right);
	up = XMVector3Normalize(up);
	XMFLOAT4 botRow(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR trans = XMLoadFloat4(&botRow);
	XMMATRIX rotation = XMMATRIX(right, up, dir, trans);

	XMFLOAT3 start = rayHitPosition;
	XMFLOAT3 end;
	XMStoreFloat3(&end, XMVectorAdd(XMLoadFloat3(&rayHitPosition), XMVectorScale(up,5.0f)));

	XMMATRIX scale = DirectX::XMMatrixScaling(boxScale.x, boxScale.y, boxScale.z);
	
	XMMATRIX localTransform = scale * rotation * translation;
	localTransform = XMMatrixMultiply(localTransform, worldToModel);
	XMStoreFloat4x4(&newDecal.localTransform, XMMatrixTranspose(localTransform));

	XMStoreFloat4x4(&newDecal.invLocalTransform, XMMatrixTranspose(XMMatrixInverse(nullptr, localTransform)));

	newDecal.type = (int)decalType;

	if (!decalsMap.count(ownerName)) {
		bool success;
		DecalBucket* b = (DecalBucket*)memAlloc->AllocateToPool((unsigned int)MEMORY_POOL::DECAL_POOL, sizeof(DecalBucket), success);
		ZeroMemory(b, sizeof(DecalBucket));
		b->owner = owner;
		decalsMap.insert({ ownerName, b });
		b->index = decalsVec.size();
		b->alive = true;
		decalsVec.push_back(b);
	}
	DecalBucket* bucket = decalsMap[ownerName];
	bucket->decals[bucket->counter] = newDecal;
	bucket->count++;
	if (bucket->count > MAX_DECALS_PER_ENTITY) bucket->count = MAX_DECALS_PER_ENTITY;
	bucket->counter++;
	bucket->counter %= MAX_DECALS_PER_ENTITY;
	return;
}

bool DecalHandler::DestroyDecalsByOwner(string owner)
{
	if(!decalsMap.count(owner)) return false;
	DecalBucket* db = decalsMap[owner];
	db->alive = false;
	decalsMap.erase(owner);
	return true;
}

/*
void DecalHandler::UpdateDecals()
{
	decalDrawList.empty();

	for (size_t i = 0; i < decalsVec.size(); i++)
	{
		DecalBucket* db = decalsVec[i];
		XMFLOAT4X4 world;
		XMFLOAT4X4 invWorld;
		XMMATRIX ownerWorld = XMMatrixTranspose(XMLoadFloat4x4(&db->owner->GetWorldMatrix()));
		XMMATRIX localWorld;
		XMMATRIX cWorld;
		for (size_t j = 0; j < db->count; j++)
		{
			localWorld = XMMatrixTranspose(XMLoadFloat4x4(&db->decals[j].localTransform));
			cWorld = XMMatrixMultiply(localWorld, ownerWorld);
			XMStoreFloat4x4(&world, XMMatrixTranspose(cWorld));
			XMStoreFloat4x4(&invWorld, XMMatrixTranspose(XMMatrixInverse(nullptr, cWorld)));

			decalDrawList.push_back({ world, invWorld, db->decals[j].type });
		}
	}
}
*/
