#pragma once
#include "ScriptManager.h"

enum class GameState
{
	Intro, MainMenu, Gameplay, GameOver, Victory
};

class GameManager : public ScriptManager
{
	map<string, Entity*>* eMap;

	GameState gs;

	const float MAX_GAME_TIME = 480.0f; // 8 minutes
	float gameTimer = MAX_GAME_TIME;

	int enemiesAlive;

	void Init();

	void Update();

public:
	void DecrementEnemiesAlive();
};

