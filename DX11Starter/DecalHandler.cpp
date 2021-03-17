#include "pch.h"
#include "DecalHandler.h"

DecalHandler* DecalHandler::instance = nullptr;
map<string, DecalBucket*> DecalHandler::decalsMap;
vector<DecalBucket*> DecalHandler::decalsVec;

DecalHandler::DecalHandler()
{
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

void DecalHandler::GenerateDecal(Entity* owner, XMFLOAT3 rayDirection, XMFLOAT3 rayHitPosition, XMFLOAT3 boxScale, DecalType decalType)
{
	string ownerName = owner->GetName();
	Decal newDecal;
	//newDecal.owner = owner;
	XMFLOAT4 decalCenter;
	//XMVECTOR calculableOwnerCenter = XMLoadFloat3(&owner->GetPosition());
	XMVECTOR calculableDecalCenter = XMLoadFloat3(&rayHitPosition);
	//calculableDecalCenter = XMVectorSubtract(calculableDecalCenter, calculableOwnerCenter);
	XMMATRIX worldToModel = XMMatrixTranspose(XMLoadFloat4x4(&owner->GetInverseWorldMatrix()));
	calculableDecalCenter = DirectX::XMVector3Transform(calculableDecalCenter, worldToModel);

	XMMATRIX translation = XMMatrixTranslationFromVector(calculableDecalCenter);

	XMFLOAT3 localRotation = XMFLOAT3(atan2(rayDirection.y, rayDirection.z), atan2(rayDirection.z, rayDirection.x), atan2(rayDirection.y, rayDirection.x));
	XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(localRotation.x, localRotation.y, localRotation.z);

	XMMATRIX scale = DirectX::XMMatrixScaling(boxScale.x, boxScale.y, boxScale.z);
	
	XMMATRIX localTransform = scale * rotation * translation;
	XMStoreFloat4x4(&newDecal.localTransform, XMMatrixTranspose(localTransform));

	XMStoreFloat4x4(&newDecal.invLocalTransform, XMMatrixTranspose(XMMatrixInverse(nullptr, localTransform)));

	newDecal.type = (int)decalType;

	if (!decalsMap.count(ownerName)) {
		bool success;
		DecalBucket* b = (DecalBucket*)MemoryAllocator::GetInstance()->AllocateToPool((unsigned int)MEMORY_POOL::DECAL_POOL, sizeof(DecalBucket), success);
		DecalBucket buc;
		*b = buc;
		b->owner = owner;
		decalsMap.insert({ ownerName, b });
		b->index = decalsVec.size();
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

bool DecalHandler::DestroyDecals(string owner)
{
	if(!decalsMap.count(owner))
		return false;
	DecalBucket* db = decalsMap[owner];
	decalsVec.erase(decalsVec.begin() + db->index);
	decalsMap.erase(owner);
	MemoryAllocator::GetInstance()->DeallocateFromPool((unsigned int)MEMORY_POOL::DECAL_POOL, db, sizeof(DecalBucket));
	return true;
}
