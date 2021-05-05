#include "pch.h"
#include "GameManager.h"

void GameManager::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;


	enemiesAlive = EESceneLoader->sceneEntitiesTagMap["Enemy"].size(); // enemies alive on game start

	totalSplitMeshEntities = std::vector<Entity*>();
	
	bloodPoolParams = {
			"Blood Pool",					// name
			"Blood Pool",					// tag
			"Blood Pool",					// layer
			"Sphere",							// mesh
			"swordgradient",							// material
			{"BLOODPOOL"},				// script names
			1,								// script count
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// position
			XMFLOAT3(0.0f, 0.0f, 0.0f),		// rotation
			XMFLOAT3(0.1f, 0.1f, 0.1f),		// scale
			0.0f,							// mass
			false
			// defaults work for the rest
	};
	
	gs = GameState::Gameplay; // For testing purposes right now, change when menus and cinematic are added to the game
}

void GameManager::Update()
{
	switch (gs)
	{
	case GameState::Intro:

		break;

	case GameState::MainMenu:
		
		break;

	case GameState::Gameplay:
		gameTimer -= deltaTime;

		// cout << "Game Timer: " << gameTimer << " Enemies Left: " << enemiesAlive << endl;
		if (gameTimer <= 0 && enemiesAlive > 0) // lose condition TODO: Change to if only if boss is alive and make enemies alive part of thes coring
		{
			gs = GameState::GameOver;
		}

		BloodPoolSpawner();

		break;

	case GameState::Victory:
		
		break;

	case GameState::GameOver:
	
		break;

	default:

		break;
	}
}

void GameManager::BloodPoolSpawner()
{
	for (int i = 0; i < totalSplitMeshEntities.size(); i++)
	{
		btVector3 from = totalSplitMeshEntities[i]->GetRBody()->getCenterOfMassPosition();
		btVector3 to = btVector3(from.getX(), from.getY() - 3.0f, from.getZ());

		btCollisionWorld::ClosestRayResultCallback closestResult = Utility::BulletRaycast(from, to);

		if (closestResult.hasHit())
		{
			PhysicsWrapper* wrapper = (PhysicsWrapper*)closestResult.m_collisionObject->getUserPointer();
			if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY) {
				Entity* e = (Entity*)wrapper->objectPointer;
				if (e->tag.STDStr() == std::string("Environment"))
				{
					bloodPoolParams.position = Utility::BulletVectorToFloat3(closestResult.m_hitPointWorld);
					ScriptManager::CreateEntity(bloodPoolParams);
					totalSplitMeshEntities.erase(totalSplitMeshEntities.begin() + i); // remove the body part from the list if it already was close enough to the ground to leave a blood puddle
				}
			}
		}
	}
}

void GameManager::DecrementEnemiesAlive()
{
	enemiesAlive--;
}

void GameManager::AddRangeToTotalSplitMeshEntities(std::vector<Entity*> splitMeshEntities)
{
	this->totalSplitMeshEntities.insert(this->totalSplitMeshEntities.end(), splitMeshEntities.begin(), splitMeshEntities.end());
}
