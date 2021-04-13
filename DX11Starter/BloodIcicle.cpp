#include "pch.h"
#include "BloodIcicle.h"

void BloodIcicle::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
	cout << "init blood icicle" << endl;
}

void BloodIcicle::Update()
{
	
}

void BloodIcicle::OnCollision(btCollisionObject* other)
{
	Entity* otherE = (Entity*)other->getUserPointer();

	cout << "Blood Icicle Hit: " << otherE->GetName().c_str() << endl;
}
