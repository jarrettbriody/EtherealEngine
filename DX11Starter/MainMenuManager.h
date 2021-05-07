#pragma once
#include "ScriptManager.h"
#include "LightHandler.h"
#include "GPUParticleEmitter.h"
#include "CPUParticleEmitter.h"
#include "SpriteFont.h"
#include "SpriteBatch.h"
#include "DecalHandler.h"

struct uiCallback : Utility::Callback {
	DirectX::SpriteFont* font;
	DirectX::SpriteBatch* spriteBatch;
	float windowWidthRatio;
	float windowHeightRatio;
	POINT windowCenter;
	float transparency = 0.0f;
	void CallbackFunc() {

		spriteBatch->Begin(SpriteSortMode_Deferred, Renderer::GetInstance()->blendState);

		XMVECTOR titleLen = font->MeasureString(L"A Demon Killed My Babushka");
		font->DrawString(
			spriteBatch,
			L"A Demon Killed My Babushka",
			XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 150.0f * windowHeightRatio + 3.0f, 0, 0),
			XMVectorSet(0, 0, 0, transparency),
			0.0f,
			XMVectorSet(titleLen.m128_f32[0] / 2.0f, titleLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(1.001f, 1.001f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			L"A Demon Killed My Babushka",
			XMVectorSet(windowCenter.x, windowCenter.y - 150.0f * windowHeightRatio, 0, 0),
			XMVectorSet(0.6f, 0, 0, transparency),
			0.0f,
			XMVectorSet(titleLen.m128_f32[0] / 2.0f, titleLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(1.0f, 1.0f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		spriteBatch->End();

		// Reset render states, since sprite batch changes these!
		Config::Context->OMSetBlendState(0, 0, 0xFFFFFFFF);
		Config::Context->OMSetDepthStencilState(0, 0);
	}
};

class MainMenuManager : public ScriptManager
{
	LightHandler* lights = nullptr;

	LightContainer* hearthLight = nullptr;

	LightContainer* menuLight = nullptr;

	Camera* cam = nullptr;

	POINT windowCenter;

	Light originalHearth;
	Light originalMenu;

	uiCallback uiCb;

	bool bloodFired = false;
	double firedTime;
	CPUParticleEmitter* splatter = nullptr;

	double totalTime = 0.0;

	~MainMenuManager();

	void Init();

	void Update();
};

