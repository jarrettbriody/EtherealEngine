#include "Barrel.h"

void Barrel::Init()
{
	cout << "initializing barrel" << endl;
}

void Barrel::Update()
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		DirectX::XMFLOAT3 trans = entity->GetPosition();
		trans.x -= 0.05f;
		entity->SetPosition(trans.x, trans.y, trans.z);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		DirectX::XMFLOAT3 trans = entity->GetPosition();
		trans.x += 0.05f;
		entity->SetPosition(trans.x, trans.y, trans.z);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState(VK_UP))
	{
		DirectX::XMFLOAT3 trans = entity->GetPosition();
		trans.z += 0.05f;
		entity->SetPosition(trans.x, trans.y, trans.z);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		DirectX::XMFLOAT3 trans = entity->GetPosition();
		trans.z -= 0.05f;
		entity->SetPosition(trans.x, trans.y, trans.z);
		entity->CalcWorldMatrix();
	}

	if (entity->CheckSATCollisionAndCorrect((*ScriptManager::sceneEntitiesMap)["barrel_1 (2)"]))
	{
		cout << test << endl;
	}
}
