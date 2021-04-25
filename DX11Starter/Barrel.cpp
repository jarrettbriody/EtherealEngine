#include "pch.h"
#include "Barrel.h"

void TestScript::Init()
{

}

void TestScript::Update()
{
	/*
	//Entity* fps = EESceneLoader->sceneEntitiesMap["FPSController"];
	Camera* cam = EERenderer->GetCamera("main");
	XMFLOAT3 camPos = cam->position;
	XMFLOAT3 camDir = cam->direction;
	entity->SetPosition(XMFLOAT3(camPos.x + camDir.x, camPos.y + camDir.y, camPos.z + camDir.z));
	entity->CalcWorldMatrix();
	*/
	/*
	//collision barrels
	if (GetAsyncKeyState(VK_LEFT))
	{
		entity->Move(-0.05f, 0, 0);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		entity->Move(0.05f, 0, 0);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState(VK_UP))
	{
		entity->Move(0, 0, 0.05f);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		entity->Move(0, 0, -0.05f);
		entity->CalcWorldMatrix();
	}
	*/

	/*
	if (entity->CheckSATCollisionAndCorrect((*eMap)["Rock (1)"]))
	{
		cout << test << endl;
	}
	*/

	/*
	//ruin
	if (GetAsyncKeyState('B') & 0x8000) {
		entity->RotateAroundAxis(Z_AXIS, -0.05f);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState('N') & 0x8000) {
		entity->RotateAroundAxis(Z_AXIS, 0.05f);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState('U') & 0x8000) {
		entity->RotateAroundAxis(X_AXIS, -0.05f);
		entity->CalcWorldMatrix();
	}
	if (GetAsyncKeyState('I') & 0x8000) {
		entity->RotateAroundAxis(X_AXIS, 0.05f);
		entity->CalcWorldMatrix();
	}

	/*
	//transform hierarchy spheres
	if (GetAsyncKeyState('F') & 0x8000) {
		(*eMap)["cube1"]->RotateAroundAxis(Y_AXIS, 0.05f);
		(*eMap)["cube1"]->CalcWorldMatrix();
	}
	if (GetAsyncKeyState('G') & 0x8000) {
		(*eMap)["cube1"]->RotateAroundAxis(Y_AXIS, -0.05f);
		(*eMap)["cube1"]->CalcWorldMatrix();
	}

	//delete barrel
	if (GetAsyncKeyState('L') & 0x8000) {
		entity->Destroy();
	}
	*/
}

void TestScript::OnCollision(btCollisionObject* other)
{
	//Entity* otherE = (Entity*)((PhysicsWrapper*)other->getUserPointer())->objectPointer;
	//cout << otherE->GetName() << endl;
}
