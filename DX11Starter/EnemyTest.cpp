#include "pch.h"
#include "EnemyTest.h"

void EnemyTest::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
	//cout << "Enemy Test attached to: " << entity->GetName() << endl;

	Entity* player = eMap->find("FPSController")->second;

	bt = BehaviorTreeBuilder()
			.Leaf<EnemySeesPlayer>(entity, player, 30.0f, 20.0f).End()
		.End();

	bt->Run();
}

void EnemyTest::Update()
{
	Status result = bt->Run();

	//if (result == SUCCESS)
	//	cout << "I see you" << endl;
}

void EnemyTest::OnCollision(btCollisionObject* other)
{
	//Entity* otherE = (Entity*)((PhysicsWrapper*)other->getUserPointer())->objectPointer;

	//cout << "Enemy collides with: " << otherE->GetName() << endl;
}
