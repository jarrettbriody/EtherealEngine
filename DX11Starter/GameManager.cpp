#include "pch.h"
#include "GameManager.h"

void GameManager::Init()
{
	uiCB.crosshair = EESceneLoader->DefaultTexturesMap["crosshair"];
	uiCB.crosshair->GetResource(&crosshairResource);
	crosshairResource->QueryInterface<ID3D11Texture2D>(&crosshairTexture);
	crosshairTexture->GetDesc(&uiCB.crosshairDesc);

	uiCB.spriteBatch = new SpriteBatch(Config::Context);
	uiCB.font = EESceneLoader->FontMap["Bloodlust"];
	//uiCB.EEMouse = Mouse::GetInstance();
	//uiCB.EESceneLoader = EESceneLoader;

	uiCB.gs = &gs;
	EERenderer->SetRenderUICallback(true, &uiCB, 0);

	eMap = ScriptManager::sceneEntitiesMap;

	enemiesAlive = EESceneLoader->SceneEntitiesTagMap["Enemy"].size(); // enemies alive on game start
	
	gs = GameState::Gameplay; // For testing purposes right now, change when menus and cinematic are added to the game

	//-------------------------------------------------------
	// AI Tests
	//-------------------------------------------------------

	NavmeshHandler* navmesh = NavmeshHandler::GetInstance();

	GridDescription desc = {};

	// Starting alley
	desc.gridID = 0;
	desc.position = XMFLOAT3(-135.0f, 100.0f, -105.0f);
	desc.size = XMFLOAT3(58.0f, 1000.0f, 280.0f);
	desc.nodeSpacing = 8.0f;
	navmesh->AddGrid(desc, true);

	// Alley before fire
	desc.gridID = 1;
	desc.position = XMFLOAT3(-325.0f, 100.0f, -250.0f);
	desc.size = XMFLOAT3(55.0f, 1000.0f, 580.0f);
	navmesh->AddGrid(desc, true);

	// Alley after fire
	desc.gridID = 2;
	desc.position = XMFLOAT3(-325.0f, 100.0f, 405.0f);
	desc.size = XMFLOAT3(55.0f, 1000.0f, 480.0f);
	navmesh->AddGrid(desc, true);

	// Connecting alley + new_moratorium (WILL PROBABLY NEED SOME ADJUSTING)
	desc.gridID = 3;
	desc.position = XMFLOAT3(-340.0f, 100.0f, 890.0f);
	desc.size = XMFLOAT3(500.0f, 1000.0f, 260.0f);
	navmesh->AddGrid(desc, true);

	// Rooftops
	desc.gridID = 4;
	desc.position = XMFLOAT3(-70.0f, 100.0f, 1250.0f);
	desc.size = XMFLOAT3(60.0f, 1000.0f, 1050.0f);
	navmesh->AddGrid(desc, true);

	// Alley after rooftops/before sewers
	desc.gridID = 5;
	desc.position = XMFLOAT3(-200.0f, 100.0f, 2305.0f);
	desc.size = XMFLOAT3(315.0f, 1000.0f, 55.0f);
	navmesh->AddGrid(desc, true);

	// Area after sewers/before boss
	desc.gridID = 6;
	desc.position = XMFLOAT3(130.0f, 100.0f, 2375.0f);
	desc.size = XMFLOAT3(230.0f, 1000.0f, 260.0f);
	navmesh->AddGrid(desc, true);

	// Boss area
	desc.gridID = 7;
	desc.position = XMFLOAT3(420.0f, 100.0f, 2370.0f);
	desc.size = XMFLOAT3(590.0f, 1000.0f, 420.0f);
	navmesh->AddGrid(desc, true);

	// Variables for sound scheduling
	unsigned int dsp_block_len, count;
	int outputrate = 0;

	// Get information needed later for scheduling.  The mixer block size, and the output rate of the mixer.
	Config::FMODResult = Config::FMODSystem->getDSPBufferSize(&dsp_block_len, 0);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	Config::FMODResult = Config::FMODSystem->getSoftwareFormat(&outputrate, 0, 0);
	AudioManager::FMODErrorCheck(Config::FMODResult);

	// Play the combat theme
	for (count = 0; count < 2; count++)
	{
		unsigned int slen;

		Config::FMODResult = Config::FMODSystem->playSound(Config::CombatTheme[count], Config::MusicGroup, true, &Config::MusicChannel); // Start a part of the combat theme, but leave it paused
		AudioManager::FMODErrorCheck(Config::FMODResult);

		Config::FMODResult = Config::MusicChannel->setVolume(Config::MusicVolume);
		AudioManager::FMODErrorCheck(Config::FMODResult);

		if (!clock_start)
		{
			Config::FMODResult = Config::MusicChannel->setChannelGroup(Config::MusicGroup);
			AudioManager::FMODErrorCheck(Config::FMODResult);

			Config::FMODResult = Config::MusicChannel->getDSPClock(0, &clock_start);
			AudioManager::FMODErrorCheck(Config::FMODResult);

			clock_start += (dsp_block_len * 2); // Set the first delay to something small, just so we can finish setting up the timing
		}
		else
		{
			float freq;
			FMOD::Sound* previousSound = Config::CombatTheme[count - 1];

			Config::FMODResult = previousSound->getLength(&slen, FMOD_TIMEUNIT_PCM);
			AudioManager::FMODErrorCheck(Config::FMODResult);

			Config::FMODResult = previousSound->getDefaults(&freq, 0);
			AudioManager::FMODErrorCheck(Config::FMODResult);

			slen = (unsigned int)((float)slen / freq * outputrate); // Calculate the length of the previously played sound

			clock_start += slen; // Set the new delay to after the previous sound finishes
		}

		Config::FMODResult = Config::MusicChannel->setDelay(clock_start, 0, false); // Set a delay for the currently playing sound
		AudioManager::FMODErrorCheck(Config::FMODResult);

		Config::FMODResult = Config::MusicChannel->setPaused(false); // Unpause this section of the combat theme so it will play after the delay passes
		AudioManager::FMODErrorCheck(Config::FMODResult);
	}
}

void GameManager::Update()
{
	//if (Keyboard::GetInstance()->KeyIsPressed((unsigned int)'T')) {
	//	EESceneLoader->SceneEntitiesMap["graveyard"]->GetTransform().RotateAroundAxis(Y_AXIS, 1.0f * deltaTime);
	//}

	RECT window;
	RECT client;
	if (GetWindowRect(Config::hWnd, &window)) {
		if (GetClientRect(Config::hWnd, &client)) {
			uiCB.windowCenter.x = round(client.left + (float)Config::ViewPortWidth / 2.0f);
			uiCB.windowCenter.y = round(client.top + (float)Config::ViewPortHeight / 2.0f);

			uiCB.windowWidthRatio = (float)Config::ViewPortWidth / 1600.0f;
			uiCB.windowHeightRatio = (float)Config::ViewPortHeight / 900.0f;
		}
	}

	switch (gs)
	{
		case GameState::Paused:
		{
			Config::UpdatePaused = true;
			Config::ShowCursor = true;
			while (ShowCursor(Config::ShowCursor) < 0);
			break;
		}

		case GameState::Gameplay:
		{
			gameTimer -= deltaTime;

			CheckTooltips();
			CheckPlayerPos();

			//// cout << "Game Timer: " << gameTimer << " Enemies Left: " << enemiesAlive << endl;
			//if (gameTimer <= 0 && enemiesAlive > 0) // lose condition TODO: Change to if only if boss is alive and make enemies alive part of thes coring
			//{
			//	gs = GameState::GameOver;
			//}
			//FPSController* playerScript = (FPSController*)scriptFunctionsMap["FPSController"]["FPSCONTROLLER"];
			//if (playerScript->GetPlayerState() == PlayerState::Death) {
			//	gs = GameState::GameOver;
			//}
			if (enemiesAlive <= 0) {
				gs = GameState::Victory;
			}

			if (Keyboard::GetInstance()->OnKeyDown(VK_ESCAPE)) {
				gs = GameState::Paused;
			}

			break;
		}
		case GameState::Victory:
		{
			Config::UpdatePaused = true;
			Config::ShowCursor = true;
			Config::CaptureMouse = false;
			while (ShowCursor(Config::ShowCursor) < 0);
			break;
		}
		case GameState::GameOver:
		{
			Config::ShowCursor = true;
			Config::CaptureMouse = false;
			while (ShowCursor(Config::ShowCursor) < 0);
			break;
		}
		default:
		{
			break;
		}
	}
}

GameManager::~GameManager()
{
	delete uiCB.spriteBatch;

	crosshairResource->Release();
	crosshairTexture->Release();
	//uiCB.crosshair->Release();
}

void GameManager::CheckTooltips()
{
	if (tooltipRampingUp) {
		if (uiCB.transparency == 1.0f) {
			tooltipTimer -= deltaTime;
			if (tooltipTimer < 0.0f) {
				tooltipRampingUp = false;
			}
		}
		else {
			uiCB.transparency += deltaTime;
			if (uiCB.transparency > 1.0f) {
				uiCB.transparency = 1.0f;
				tooltipTimer = originalTooltipTimer;
			}
		}
	}
	else {
		uiCB.transparency -= deltaTime;
		if (uiCB.transparency < 0.0f) uiCB.transparency = 0.0f;
	}
	
	if (tooltipIndex < 7) {
		if (XMVector3LengthSq(XMVectorSubtract(XMLoadFloat3(&EESceneLoader->SceneEntitiesMap["FPSController"]->GetTransform().GetPosition()), XMLoadFloat3(&tooltipLocations[tooltipIndex]))).m128_f32[0] <= tooltipDistance) {
			uiCB.transparency = 0.0f;
			uiCB.tooltip = tooltips[tooltipIndex];
			tooltipIndex++;
			tooltipRampingUp = true;
		}
	}
}

void GameManager::CheckPlayerPos()
{
	Entity* player = EESceneLoader->SceneEntitiesMap["FPSController"];
	if (player->GetTransform().GetPosition().y < -100.0f) {
		unsigned int index = tooltipIndex;
		player->GetTransform().SetPosition(tooltipLocations[index - 1]);
		player->GetRBody()->clearForces();
		player->GetRBody()->setLinearVelocity(btVector3(0, 0, 0));
	}
}

void GameManager::DecrementEnemiesAlive()
{
	enemiesAlive--;
}
