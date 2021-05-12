#include "pch.h"
#include "BullEnemy.h"

BullEnemy::~BullEnemy()
{
	delete bt;
}

void BullEnemy::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
	sMap = ScriptManager::scriptFunctionsMapVector;

	//sMap["FPSCONTROLLER"].front()
	fpsControllerScript = (FPSController*)(scriptFunctionsMap["FPSController"]["FPSCONTROLLER"]);
	gameManagerScript = (GameManager*)(scriptFunctionsMap["GameManager"]["GAMEMANAGER"]);

	//grid = &controller->grid;

	entity->AddTag(std::string("Bull"));

	Entity* player = eMap->find("FPSController")->second;

	pos = entity->GetTransform().GetPosition();

	/*bt = BehaviorTreeBuilder()
			.Leaf<EnemySeesPlayer>(entity, player, 30.0f, 30.0f).End()
		.End();*/

	entity->GetRBody()->setAngularFactor(btVector3(0, 1, 0)); // Constrain rotations on x and z axes
	entity->GetRBody()->setLinearFactor(btVector3(1, 0, 1)); // Constrain movement on the y axis

	bt =	BehaviorTreeBuilder()
				.Composite<ActiveSelector>()
					.Composite<Sequence>() // Charge the player if they are in range
						.Leaf<InCombat>(&inCombat).End()
						.Leaf<PlayerVisible>(entity, player).End()
						.Leaf<PlayerIsInRange>(entity, player, minimumDistance, &playerIsInRange).End()
						.Leaf<AbilityAvailable>(&chargeCooldownTimer).End()
						.Leaf<FacePlayer>(entity, player, turnSpeed, &deltaTime).End()
						.Leaf<ChargePlayer>(entity, player, chargeSpeed).End()
					.End()
					.Composite<Sequence>() // Seek the player if they are visible
						.Leaf<InCombat>(&inCombat).End()
						.Leaf<PlayerVisible>(entity, player).End()
						.Leaf<FacePlayer>(entity, player, turnSpeed, &deltaTime).End()
						.Leaf<AbilityAvailable>(&chargeCooldownTimer).End()
						.Leaf<SeekPlayer>(entity, player, movementSpeed, maxSpeed, minimumDistance, &playerIsInRange).End()
					.End()
					.Composite<Sequence>() // Search player's last known location
						.Leaf<InCombat>(&inCombat).End()
						.Decorator<Invert>()
							.Leaf<PlayerVisible>(entity, player).End()
						.End()
						.Leaf<FindPlayer>(entity, player, &aStarSolver, &path).End()
						.Leaf<FollowPath>(&path, entity, movementSpeed, minimumDistance, turnSpeed, &deltaTime).End()
						.Leaf<Idle>(entity, &inCombat).End()
					.End()
					.Composite<Sequence>() // Enemy idle
						.Decorator<Invert>()
							.Leaf<InCombat>(&inCombat).End()
						.End()
						.Leaf<Idle>(entity, &inCombat).End()
						.Leaf<EnemySeesPlayer>(entity, player, visionConeAngle, visionConeDistance, &inCombat).End()
					.End()
				.End()
			.End();

	keyboard = Keyboard::GetInstance();
	navmesh = NavmeshHandler::GetInstance();
}

void BullEnemy::Update()
{
	// Hover
	//totalTime += deltaTime;
	//pos.y = pos.y + sin(totalTime) * oscillationMagnitude;
	//entity->SetPosition(pos);
	//entity->CalcWorldMatrix();

	CheckPlayerState();

	if (chargeCooldownTimer > 0)
	{
		chargeCooldownTimer -= deltaTime;
	}

	// TODO: Reset the enemy transformation properly after leash is over
	if (delay <= 0)
	{
		if (leashed)
		{
			entity->GetRBody()->setAngularFactor(btVector3(1, 1, 1));
			entity->GetRBody()->setLinearFactor(btVector3(1, 1, 1));
		}
		else
		{
			entity->GetRBody()->setAngularFactor(btVector3(0, 1, 0)); // Constrain rotations on x and z axes
			entity->GetRBody()->setLinearFactor(btVector3(1, 0, 1)); // Constrain movement on the y axis

			Status result = bt->Run();
		}
	}
	else
	{
		delay -= deltaTime;
	}
}

void BullEnemy::OnCollision(btCollisionObject* other)
{
	Entity* otherE = (Entity*)((PhysicsWrapper*)other->getUserPointer())->objectPointer;

	// cout << "Enemy collides with: " << otherE->GetName() << endl;

	// kill if slamming into the wall while leashed
	if (otherE->HasTag(std::string("Environment")) && !otherE->HasTag(std::string("street")) && entity->GetRBody()->getLinearVelocity().length() > killSpeedWhileLeashed)
	{
		// Store the old enemy position for later use in case the enemy was killed while leashed
		btVector3 oldEnemyPos = entity->GetRBody()->getCenterOfMassPosition();

		// enemy is in the triangle, split it apart
		std::vector<Entity*> childEntities = EESceneLoader->SplitMeshIntoChildEntities(entity, 10.0f, "BODYPART");

		// Update the game manager attribute for enemies alive
		gameManagerScript->DecrementEnemiesAlive();

		Entity* newLeashedEntity = childEntities[0];
		for each (Entity * e in childEntities)
		{
			e->AddTag(std::string("Body Part"));

			e->GetRBody()->activate();
			//e->GetRBody()->applyCentralImpulse(btVector3(100, 100, 100));
			//e->GetRBody()->applyTorqueImpulse(btVector3(100, 100, 100));

			if (leashed)
			{
				if (e->GetRBody()->getCenterOfMassPosition().distance(oldEnemyPos) < newLeashedEntity->GetRBody()->getCenterOfMassPosition().distance(oldEnemyPos))
				{
					newLeashedEntity = e;
				}
			}
		}

		gameManagerScript->AddRangeToTotalSplitMeshEntities(childEntities);
		if (leashed) fpsControllerScript->SetLeashedEntity(newLeashedEntity);
	}

	if (otherE->GetName().c_str() == std::string("FPSController") && chargeCooldownTimer <= 0)
	{
		entity->GetRBody()->activate();
		entity->GetRBody()->setLinearVelocity(btVector3(0, 0, 0));

		chargeCooldownTimer = CHARGE_COOLDOWN_MAX;
	}
}

void BullEnemy::IsLeashed(bool leashed, float delay)
{
	this->leashed = leashed;
	this->delay = delay;
}

void BullEnemy::CheckPlayerState()
{
	if (fpsControllerScript->GetPlayerState() == PlayerState::Death) inCombat = false;
}
