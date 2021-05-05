#include "pch.h"
#include "GameManager.h"

void GameManager::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	std::vector<Entity*> enemies = EESceneLoader->SceneEntitiesTagMap["Enemy"];

	enemiesAlive = enemies.size();

	gs = GameState::Gameplay; // For testing purposes right now, change when menus and cinematic are added to the game

	//-------------------------------------------------------
	// AI Tests
	//-------------------------------------------------------
	/*
	// Starting alley
	XMFLOAT3 gridStart(-135.0f, 100.0f, -105.0f);
	XMFLOAT2 gridSize(58.0f, 280.0f);
	grid1 = Grid(gridStart, gridSize, 6.0f);
	//grid1.CreateGrid();

	// Alley before fire
	gridStart = XMFLOAT3(-325.0f, 100.0f, -250.0f);
	gridSize = XMFLOAT2(55.0f, 580.0f);
	grid2 = Grid(gridStart, gridSize, 6.0f);
	//grid2.CreateGrid();

	// Alley after fire
	gridStart = XMFLOAT3(-325.0f, 100.0f, 405.0f);
	gridSize = XMFLOAT2(55.0f, 480.0f);
	grid3 = Grid(gridStart, gridSize, 6.0f);
	//grid3.CreateGrid();

	// Connecting alley + new_moratorium (WILL PROBABLY NEED SOME ADJUSTING)
	gridStart = XMFLOAT3(-340.0f, 100.0f, 890.0f);
	gridSize = XMFLOAT2(500.0f, 260.0f);
	grid4 = Grid(gridStart, gridSize, 6.0f);
	//grid4.CreateGrid();

	// Rooftops
	gridStart = XMFLOAT3(-70.0f, 100.0f, 1250.0f);
	gridSize = XMFLOAT2(60.0f, 1050.0f);
	grid5 = Grid(gridStart, gridSize, 6.0f);
	//grid5.CreateGrid();

	// Alley after rooftops/before sewers
	gridStart = XMFLOAT3(-200.0f, 100.0f, 2305.0f);
	gridSize = XMFLOAT2(315.0f, 55.0f);
	grid6 = Grid(gridStart, gridSize, 6.0f);
	//grid6.CreateGrid();

	// Area after sewers/before boss
	gridStart = XMFLOAT3(130.0f, 100.0f, 2375.0f);
	gridSize = XMFLOAT2(230.0f, 260.0f);
	grid7 = Grid(gridStart, gridSize, 6.0f);
	//grid7.CreateGrid();

	// Boss area
	gridStart = XMFLOAT3(420.0f, 100.0f, 2370.0f);
	gridSize = XMFLOAT2(590.0f, 420.0f);
	grid8 = Grid(gridStart, gridSize, 6.0f);
	grid8.CreateGrid();
	*/
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

		//cout << "Game Timer: " << gameTimer << " Enemies Left: " << enemiesAlive << endl;
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
