#include "pch.h"
#include "DecalHandler.h"

DecalHandler* DecalHandler::instance = nullptr;
map<string, DecalBucket> DecalHandler::decalsMap;

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

bool DecalHandler::GenerateDecal(Entity* owner, XMFLOAT3 rayDirection, XMFLOAT3 rayHitPosition, XMFLOAT3 boxScale, DecalType decalType)
{
	string ownerName = owner->GetName();
	Decal newDecal;
	//newDecal.owner = owner;
	XMFLOAT4 decalCenter;
	XMVECTOR calculableOwnerCenter = XMLoadFloat3(&owner->GetPosition());
	XMVECTOR calculableDecalCenter = XMLoadFloat3(&rayHitPosition);
	calculableDecalCenter = XMVectorSubtract(calculableDecalCenter, calculableOwnerCenter);
	XMMATRIX worldToModel = owner->CalcWorldToModelMatrix();
	calculableDecalCenter = DirectX::XMVector3Transform(calculableDecalCenter, worldToModel);

	XMMATRIX translation = XMMatrixTranslationFromVector(calculableDecalCenter);

	XMFLOAT3 localRotation = XMFLOAT3(atan2(rayDirection.y, rayDirection.z), atan2(rayDirection.z, rayDirection.x), atan2(rayDirection.y, rayDirection.x));
	XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(localRotation.x, localRotation.y, localRotation.z);

	XMMATRIX scale = DirectX::XMMatrixScaling(boxScale.x, boxScale.y, boxScale.z);
	
	XMMATRIX localTransform = scale * rotation * translation;
	XMStoreFloat4x4(&newDecal.localTransform, localTransform);

	newDecal.type = (int)decalType;

	if (!decalsMap.count(ownerName)) {
		DecalBucket b;
		decalsMap.insert({ ownerName, b });
	}
	DecalBucket& bucket = decalsMap[ownerName];
	bucket.decals[bucket.counter] = newDecal;
	bucket.count++;
	if (bucket.count > MAX_DECALS_PER_ENTITY) bucket.count = MAX_DECALS_PER_ENTITY;
	bucket.counter++;
	bucket.counter %= MAX_DECALS_PER_ENTITY;
	return true;
}

bool DecalHandler::DestroyDecals(string owner)
{
	if(!decalsMap.count(owner))
		return false;
	decalsMap.erase(owner);
	return true;
}
