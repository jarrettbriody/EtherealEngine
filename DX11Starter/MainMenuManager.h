#pragma once
#include "ScriptManager.h"
#include "LightHandler.h"
#include "GPUParticleEmitter.h"
#include "SpriteFont.h"
#include "SpriteBatch.h"

struct uiCallback : Utility::Callback {
	DirectX::SpriteFont* font;
	DirectX::SpriteBatch* spriteBatch;
	void CallbackFunc() {

	}
};

class MainMenuManager : public ScriptManager
{
	LightHandler* lights = nullptr;

	LightContainer* hearthLight = nullptr;

	LightContainer* menuLight = nullptr;

	Light originalHearth;
	Light originalMenu;

	uiCallback uiCb;

	double totalTime = 0.0;

	void Init();

	void Update();
};

