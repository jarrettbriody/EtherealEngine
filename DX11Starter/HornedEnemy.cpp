#include "pch.h"
#include "HornedEnemy.h"

HornedEnemy::~HornedEnemy()
{
	delete bt;
}

void HornedEnemy::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;
	sMap = ScriptManager::scriptFunctionsMapVector;

	//sMap["FPSCONTROLLER"].front()
	fpsControllerScript = (FPSController*)(scriptFunctionsMap["FPSController"]["FPSCONTROLLER"]);
	//grid = &controller->grid;

	entity->AddTag(std::string("Horned"));

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
						.Leaf<SeekPlayer>(entity, player, movementSpeed, maxSpeed, minimumDistance, &playerIsInRange).End()
						.Leaf<PlayerIsInRange>(&playerIsInRange).End()
						.Leaf<AbilityAvailable>(&houndCooldownTimer).End()
						.Leaf<HoundPlayer>(entity, player, houndSpeed, &houndCooldownTimer, HOUND_COOLDOWN_MAX).End()
					.End()
					.Composite<Sequence>() // Search player's last known location
						.Leaf<InCombat>(&inCombat).End()
						.Leaf<FindPlayer>(entity, player, &aStarSolver, &path).End()
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
	navmesh = NavmeshHandler::GetInstance();
}

void HornedEnemy::Update()
{
	// Hover
	//totalTime += deltaTime;
	//pos.y = pos.y + sin(totalTime) * oscillationMagnitude;
	//entity->SetPosition(pos);
	//entity->CalcWorldMatrix();

	if (houndCooldownTimer > 0)
	{
		houndCooldownTimer -= deltaTime;
	}

	CheckPlayerState();

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

void HornedEnemy::OnCollision(btCollisionObject* other)
{
	//Entity* otherE = (Entity*)((PhysicsWrapper*)other->getUserPointer())->objectPointer;

	//cout << "Enemy collides with: " << otherE->GetName() << endl;
}

void HornedEnemy::IsLeashed(bool leashed, float delay)
{
	this->leashed = leashed;
	this->delay = delay;
}

void HornedEnemy::CheckPlayerState()
{
	if (fpsControllerScript->GetPlayerState() == PlayerState::Death) inCombat = false;
}
