#include "pch.h"
#include "Barrel.h"

void TestScript::Init()
{
	/*
	cout << "initializing barrel" << endl;
	(*eMap)["barrel_1"]->isCollisionStatic = false;
	(*eMap)["barrel_1 (2)"]->isCollisionStatic = false;
	*/
}

void TestScript::Update()
{
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

	Material* mat = entity->GetMaterial(entity->GetMaterialNameList()[0]);
	SimpleVertexShader* vertShader = mat->GetVertexShader();
	vertShader->SetFloat("fillLineY", 0.25f);
	vertShader->SetFloat("totalTime", totalTime);
	vertShader->SetFloat("deltaTime", deltaTime);
	vertShader->SetFloat("waveHeight", 0.05f);
	vertShader->SetInt("counter", waveCounter);
	waveCounter++;
	totalTime += deltaTime;
	/*
	if (entity->CheckSATCollisionAndCorrect((*eMap)["Rock (1)"]))
	{
		cout << test << endl;
	}
	//ruin
	if (GetAsyncKeyState('B') & 0x8000) {
		(*eMap)["Ruin"]->RotateAroundAxis(Y_AXIS, -0.05f);
		(*eMap)["Ruin"]->CalcWorldMatrix();
	}
	if (GetAsyncKeyState('N') & 0x8000) {
		(*eMap)["Ruin"]->RotateAroundAxis(Y_AXIS, 0.05f);
		(*eMap)["Ruin"]->CalcWorldMatrix();
	}

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
