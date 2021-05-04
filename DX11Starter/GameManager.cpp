#include "pch.h"
#include "GameManager.h"

void GameManager::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	std::vector<Entity*> enemies = EESceneLoader->SceneEntitiesTagMap["Enemy"];

	enemiesAlive = enemies.size();

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

		cout << "Game Timer: " << gameTimer << " Enemies Left: " << enemiesAlive << endl;
		if (gameTimer <= 0 && enemiesAlive > 0) // lose condition
		{
			gs = GameState::GameOver;
		}

		break;

	case GameState::Victory:
		
		break;

	case GameState::GameOver:
	
		break;

	default:

		break;
	}
}

void GameManager::DecrementEnemiesAlive()
{
	enemiesAlive--;
}
