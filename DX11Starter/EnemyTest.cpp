#include "pch.h"
#include "EnemyTest.h"

EnemyTest::~EnemyTest()
{
	delete bt;
}

void EnemyTest::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
	sMap = ScriptManager::scriptFunctionsMapVector;

	//sMap["FPSCONTROLLER"].front()
	FPSController* controller = (FPSController*)(scriptFunctionsMap["FPSController"]["FPSCONTROLLER"]);;
	grid = &controller->grid;

	Entity* player = eMap->find("FPSController")->second;

	pos = entity->GetPosition();

	/*bt = BehaviorTreeBuilder()
			.Leaf<EnemySeesPlayer>(entity, player, 30.0f, 30.0f).End()
		.End();*/

	entity->GetRBody()->setAngularFactor(btVector3(0, 1, 0)); // Constrain rotations on x and z axes
	entity->GetRBody()->setLinearFactor(btVector3(1, 0, 1)); // Constrain movement on the y axis

	bt =	BehaviorTreeBuilder()
				.Composite<ActiveSelector>()
					.Composite<Sequence>() // Seek the player
						.Leaf<InCombat>(&inCombat).End()
						.Leaf<PlayerVisible>(entity, player).End()
						.Leaf<FacePlayer>(entity, player, turnSpeed, &deltaTime).End()
						//.Leaf<SeekPlayer>(entity, player, movementSpeed, maxSpeed, minimumDistance).End()
					.End()
					.Leaf<EnemySeesPlayer>(entity, player, visionConeAngle, visionConeDistance, &inCombat).End() // Search for player
				.End()
			.End();

	keyboard = Keyboard::GetInstance();
}

void EnemyTest::Update()
{
	// Hover
	//entity->Move(0.0f, 0.0f, 0.0f);
	totalTime += deltaTime;
	pos.y = pos.y + sin(totalTime) * oscillationMagnitude;
	entity->SetPosition(pos);
	entity->CalcWorldMatrix();

	if (keyboard->KeyIsPressed(0x4A)) // J
	{
		XMFLOAT3 ePos = entity->GetPosition();
		Node* closest = grid->FindNearestNode(ePos);
		XMFLOAT3 nodePos = closest->GetPos();
		cout << "Enemy position- X: " << ePos.x << "| Z: " << ePos.z << endl;
		cout << "Node position- X: " << nodePos.x << "| Z: " << nodePos.z << endl;
	}

	Status result = bt->Run();
}

void EnemyTest::OnCollision(btCollisionObject* other)
{
	//Entity* otherE = (Entity*)((PhysicsWrapper*)other->getUserPointer())->objectPointer;

	//cout << "Enemy collides with: " << otherE->GetName() << endl;
}
