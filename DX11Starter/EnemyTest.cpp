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
					.Composite<Sequence>() // Seek the player if they are visible
						.Leaf<InCombat>(&inCombat).End()
						.Leaf<PlayerVisible>(entity, player).End()
						.Leaf<FacePlayer>(entity, player, turnSpeed, &deltaTime).End()
						.Leaf<SeekPlayer>(entity, player, movementSpeed, maxSpeed, minimumDistance).End()
					.End()
					.Composite<Sequence>() // Search player's last known location
						.Leaf<InCombat>(&inCombat).End()
						.Leaf<FindPlayer>(entity, player, grid, &path).End()
						.Leaf<FollowPath>(&path, entity, movementSpeed, minimumDistance, turnSpeed, &deltaTime).End()
						.Leaf<Idle>(entity, &inCombat).End()
					.End()
					.Composite<Sequence>() // Enemy idle
						.Leaf<Idle>(entity, &inCombat).End()
						.Leaf<EnemySeesPlayer>(entity, player, visionConeAngle, visionConeDistance, &inCombat).End()
					.End()
				.End()
			.End();

	keyboard = Keyboard::GetInstance();
}

void EnemyTest::Update()
{
	// Hover
	//totalTime += deltaTime;
	//pos.y = pos.y + sin(totalTime) * oscillationMagnitude;
	//entity->SetPosition(pos);
	//entity->CalcWorldMatrix();

	if (keyboard->KeyIsPressed(0x4A)) // J
	{
		XMFLOAT3 ePos = entity->GetPosition();
		Node* closest = grid->FindNearestNode(ePos);
		XMFLOAT3 nodePos = closest->GetPos();
		cout << "Enemy position- X: " << ePos.x << "| Z: " << ePos.z << endl;
		cout << "Node position- X: " << nodePos.x << "| Z: " << nodePos.z << endl;
		
		clock_t t;
		Entity* player = eMap->find("FPSController")->second;
		t = clock();
		path = grid->FindPath(entity->GetPosition(), player->GetPosition());
		t = clock() - t;
		printf("It took me %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);

		//cout << "A* took " << t << " second(s)" << endl;
		cout << "Path was actually " << path.size() << " node(s) long. Whoops!" << endl;

		if (path.size() > 0)
		{
			XMFLOAT4X4 wm;
			XMStoreFloat4x4(&wm, XMMatrixTranspose(DirectX::XMMatrixIdentity()));

			// Create debug line
			DebugLines* dl = new DebugLines("TestRay", 0, false);
			XMFLOAT3 c;
			c = XMFLOAT3(0.0f, 1.0f, 0.0f);

			dl->color = c;
			dl->worldMatrix = wm;

			XMFLOAT3 start = XMFLOAT3(path.back()->GetPos().x, 10.0f, path.back()->GetPos().z);
			XMFLOAT3 end = XMFLOAT3(start.x, 0.0f, start.z);

			// Draw the debug line to show the raycast
			XMFLOAT3* rayPoints = new XMFLOAT3[8];
			rayPoints[0] = start;
			rayPoints[1] = start;
			rayPoints[2] = start;
			rayPoints[3] = start;
			rayPoints[4] = end;
			rayPoints[5] = end;
			rayPoints[6] = end;
			rayPoints[7] = end;
			dl->GenerateCuboidVertexBuffer(rayPoints, 8);
			delete[] rayPoints;
		}
	}

	Status result = bt->Run();
}

void EnemyTest::OnCollision(btCollisionObject* other)
{
	//Entity* otherE = (Entity*)((PhysicsWrapper*)other->getUserPointer())->objectPointer;

	//cout << "Enemy collides with: " << otherE->GetName() << endl;
}
