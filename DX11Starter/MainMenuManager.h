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
	float otherTransparency = 0.0f;
	void Call() {

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

		XMVECTOR startLen = font->MeasureString(L"START");;
		font->DrawString(
			spriteBatch,
			L"START",
			XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 80.0f * windowHeightRatio + 3.0f, 0, 0),
			XMVectorSet(0.4f, 0, 0, otherTransparency),
			0.0f,
			XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(0.751f, 0.751f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			L"START",
			XMVectorSet(windowCenter.x, windowCenter.y - 80.0f * windowHeightRatio, 0, 0),
			XMVectorSet(0, 0, 0, otherTransparency),
			0.0f,
			XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(0.75f, 0.75f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		startLen = font->MeasureString(L"OPTIONS");;
		font->DrawString(
			spriteBatch,
			L"OPTIONS",
			XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 40.0f * windowHeightRatio + 3.0f, 0, 0),
			XMVectorSet(0.4f, 0, 0, otherTransparency),
			0.0f,
			XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(0.751f, 0.751f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			L"OPTIONS",
			XMVectorSet(windowCenter.x, windowCenter.y - 40.0f * windowHeightRatio, 0, 0),
			XMVectorSet(0, 0, 0, otherTransparency),
			0.0f,
			XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(0.75f, 0.75f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		startLen = font->MeasureString(L"CREDITS");;
		font->DrawString(
			spriteBatch,
			L"CREDITS",
			XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 0.0f * windowHeightRatio + 3.0f, 0, 0),
			XMVectorSet(0.4f, 0, 0, otherTransparency),
			0.0f,
			XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(0.751f, 0.751f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			L"CREDITS",
			XMVectorSet(windowCenter.x, windowCenter.y - 0.0f * windowHeightRatio, 0, 0),
			XMVectorSet(0, 0, 0, otherTransparency),
			0.0f,
			XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(0.75f, 0.75f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		startLen = font->MeasureString(L"QUIT");;
		font->DrawString(
			spriteBatch,
			L"QUIT",
			XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 40.0f * windowHeightRatio + 3.0f, 0, 0),
			XMVectorSet(0.4f, 0, 0, otherTransparency),
			0.0f,
			XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(0.751f, 0.751f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			L"QUIT",
			XMVectorSet(windowCenter.x, windowCenter.y + 40.0f * windowHeightRatio, 0, 0),
			XMVectorSet(0, 0, 0, otherTransparency),
			0.0f,
			XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(0.75f, 0.75f, 0, 0),
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

	double timeOffsetsRotation[5] = {0.0,1.0,2.0,2.0,1.5};
	double timeOffsetPos = 6.5;

	XMFLOAT3 rotations[5] = {XMFLOAT3(0.64f,1.1108f,0),XMFLOAT3(1,0,0),XMFLOAT3(-0.24f,0.800797f, 0),XMFLOAT3(-0.12f,2.5908f, 0),XMFLOAT3(0,0,-1)};
	int rotCounter = 0;
	double rotTimeStamp = 0.0;
	XMFLOAT3 positions[4] = { XMFLOAT3(-6.36816f, 3.55001f, -1.79395f),XMFLOAT3(-5.83396f, 4.09953f, -1.79395f),XMFLOAT3(-3.66047f, 4.66161f, -1.79395f),XMFLOAT3(0.31f,4.95f,-1.79395f) };
	int posCounter = 0;

	~MainMenuManager();

	void Init();

	void Update();
};

