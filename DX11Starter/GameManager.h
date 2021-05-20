#pragma once
#include "ScriptManager.h"
#include "NavmeshHandler.h"
#include "AudioManager.h"
#include "SpriteFont.h"
#include "SpriteBatch.h"

enum class GameState
{
	Intro, MainMenu, Gameplay, GameOver, Victory
};

struct InGameUICallback : Utility::Callback {
	DirectX::SpriteFont* font;
	DirectX::SpriteBatch* spriteBatch;
	ID3D11ShaderResourceView* crosshair;
	D3D11_TEXTURE2D_DESC crosshairDesc;
	string playerPos;
	float windowWidthRatio;
	float windowHeightRatio;
	POINT windowCenter;

	float transparency = 0.0f;
	float otherTransparency = 0.0f;
	LPCWSTR tooltip;

	void DrawCrosshair() {
		//RECT rectangle = RECT();

		spriteBatch->Draw(
			crosshair,
			XMVectorSet(windowCenter.x, windowCenter.y,0,0),
			nullptr,
			XMVectorSet(1, 1, 1, 1),
			0.0f,
			XMVectorSet(ceil((float)crosshairDesc.Width / 2.0f), ceil((float)crosshairDesc.Height / 2.0f), 0, 0),
			XMVectorSet(1.0f, 1.0f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
	}

	void DrawInGameUI() {
		XMVECTOR titleLen = font->MeasureString(tooltip);
		font->DrawString(
			spriteBatch,
			tooltip,
			XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 400.0f * windowHeightRatio + 3.0f, 0, 0),
			XMVectorSet(0, 0, 0, transparency),
			0.0f,
			XMVectorSet(titleLen.m128_f32[0] / 2.0f, titleLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(0.401f * windowWidthRatio, 0.401f * windowHeightRatio, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			tooltip,
			XMVectorSet(windowCenter.x, windowCenter.y - 400.0f * windowHeightRatio, 0, 0),
			XMVectorSet(0.5f, 0, 0, transparency),
			0.0f,
			XMVectorSet(titleLen.m128_f32[0] / 2.0f, titleLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(0.4f * windowWidthRatio, 0.4f * windowHeightRatio, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
	}

	void Call()
	{
		spriteBatch->Begin(SpriteSortMode_Deferred, Renderer::GetInstance()->blendState);

		DrawCrosshair();
		if(transparency > 0.0f) DrawInGameUI();

		spriteBatch->End();

		// Reset render states, since sprite batch changes these!
		Config::Context->OMSetBlendState(0, 0, 0xFFFFFFFF);
		Config::Context->OMSetDepthStencilState(0, 0);
	}
};

class GameManager : public ScriptManager
{
	InGameUICallback uiCB;

	map<string, Entity*>* eMap;

	GameState gs;

	const float MAX_GAME_TIME = 480.0f; // 8 minutes
	float gameTimer = MAX_GAME_TIME;

	int enemiesAlive;

	XMFLOAT3 tooltipLocations[7] = { XMFLOAT3(11,2,0), XMFLOAT3(-77,2,0), XMFLOAT3(-100,2,136), XMFLOAT3(-302,2,132), XMFLOAT3(-243,2,132), XMFLOAT3(-297,2,313), XMFLOAT3(-22,16,1087) };
	LPCWSTR tooltips[7] = {
		L"Babushka misses you dearly sweetheart. Press WASD to move, and LMB to swing your sword.", 
		L"Remember how we crossed the stream in our backyard? Press SPACE to jump, and again to double jump.",
		L"Watch your health love, the blood in your blood orb is your life! Refill it with demon blood pools!",
		L"You can shoot your blood icicle with RMB, but don't forget that it comes at the cost of your blood!",
		L"Oh my I almost forgot! Dash with SHIFT! Your blood rings represent how many dashes you have.",
		L"Watch out for the fire! Dash over it or use your grapple hook with 'E'!",
		L"Don't forget! You can use your grapple hook with the 'E' key!"
	};
	unsigned int tooltipIndex = 0;
	float tooltipDistance = 2500.0f;
	const float originalTooltipTimer = 8.0f;
	float tooltipTimer = 0.0f;
	bool tooltipRampingUp = true;

	void Init();

	void Update();

	~GameManager();

	void CheckTooltips();
	void CheckPlayerPos();

public:
	// AI
	Grid grid1;
	Grid grid2;
	Grid grid3;
	Grid grid4;
	Grid grid5;
	Grid grid6;
	Grid grid7;
	Grid grid8;

	void DecrementEnemiesAlive();
};

