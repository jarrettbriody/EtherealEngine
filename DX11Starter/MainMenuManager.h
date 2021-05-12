#pragma once
#include "ScriptManager.h"
#include "LightHandler.h"
#include "GPUParticleEmitter.h"
#include "CPUParticleEmitter.h"
#include "SpriteFont.h"
#include "SpriteBatch.h"
#include "DecalHandler.h"
#include "Mouse.h"
#include "AudioManager.h"

struct uiCallback : Utility::Callback {
	DirectX::SpriteFont* font;
	DirectX::SpriteBatch* spriteBatch;
	float windowWidthRatio;
	float windowHeightRatio;
	POINT windowCenter;
	float transparency = 0.0f;
	float otherTransparency = 0.0f;
	SceneLoader* EESceneLoader = nullptr;
	Mouse* EEMouse = nullptr;

	float skipTooltipTransparency = 0.0f;

	bool AABB(XMVECTOR position, XMVECTOR origin, XMVECTOR scale) {
		XMFLOAT2 start(position.m128_f32[0] - origin.m128_f32[0] * scale.m128_f32[0], position.m128_f32[1] - origin.m128_f32[1] * scale.m128_f32[1]);
		XMFLOAT2 end(position.m128_f32[0] + origin.m128_f32[0] * scale.m128_f32[0], position.m128_f32[1] + origin.m128_f32[1] * scale.m128_f32[1]);
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(Config::hWnd, &mousePos);
		return mousePos.x >= start.x && mousePos.x <= end.x && mousePos.y >= start.y && mousePos.y <= end.y;
	}

	void DrawMainMenu() {
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

		XMVECTOR frontColorNormal = XMVectorSet(0, 0, 0, otherTransparency);
		XMVECTOR frontColorHighlight = XMVectorSet(0, 0, 0, otherTransparency - 0.4f);

		XMVECTOR backColorNormal = XMVectorSet(0.2f, 0, 0, otherTransparency);
		XMVECTOR backColorHighlight = XMVectorSet(0.2f, 0, 0, otherTransparency - 0.4f);

		LPCWSTR str = L"START";

		XMVECTOR startLen = font->MeasureString(str);

		XMVECTOR origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		XMVECTOR posFront = XMVectorSet(windowCenter.x, windowCenter.y - 60.0f * windowHeightRatio, 0, 0);
		XMVECTOR scaleFront = XMVectorSet(0.75f, 0.75f, 0, 0);
		XMVECTOR colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		XMVECTOR posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 60.0f * windowHeightRatio + 3.0f, 0, 0);
		XMVECTOR scaleBack = XMVectorSet(0.751f, 0.751f, 0, 0);
		XMVECTOR colorBack = (AABB(posFront, origin, scaleFront)) ? backColorHighlight : backColorNormal;

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			colorBack,
			0.0f,
			origin,
			scaleBack,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			str,
			posFront,
			colorFront,
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		if (AABB(posFront, origin, scaleFront) && otherTransparency >= 0.3f) {
			if (EEMouse->LMBIsPressed()) {
				/*
				// Variable for sound scheduling
				int outputrate = 0;

				// Get the output rate of the mixer for scheduling
				Config::FMODResult = Config::FMODSystem->getSoftwareFormat(&outputrate, 0, 0);
				AudioManager::FMODErrorCheck(Config::FMODResult);

				static unsigned long long clock_start = 0;

				Config::FMODResult = Config::MusicChannel->getDSPClock(0, &clock_start);
				AudioManager::FMODErrorCheck(Config::FMODResult);

				// Fade out the main theme
				Config::FMODResult = Config::MusicGroup->addFadePoint(clock_start, 1.0f);
				AudioManager::FMODErrorCheck(Config::FMODResult);
				clock_start += (outputrate * 1);
				Config::FMODResult = Config::MusicGroup->addFadePoint(clock_start, 0.0f);
				AudioManager::FMODErrorCheck(Config::FMODResult);

				Config::FMODResult = Config::MusicGroup->setDelay(0, clock_start, true); // Stop the main theme after we fade out
				AudioManager::FMODErrorCheck(Config::FMODResult);*/

				Config::FMODResult = Config::MusicGroup->stop();
				AudioManager::FMODErrorCheck(Config::FMODResult);

				EESceneLoader->SetModelPath("../../Assets/Models/Kamchatka/");
				EESceneLoader->LoadScene("Kamchatka");
				//return;
			}
		}

		str = L"OPTIONS";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 20.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 20.0f * windowHeightRatio + 3.0f, 0, 0);
		colorBack = (AABB(posFront, origin, scaleFront)) ? backColorHighlight : backColorNormal;

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			colorBack,
			0.0f,
			origin,
			scaleBack,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			str,
			posFront,
			colorFront,
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"CREDITS";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 20.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 20.0f * windowHeightRatio + 3.0f, 0, 0);
		colorBack = (AABB(posFront, origin, scaleFront)) ? backColorHighlight : backColorNormal;

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			colorBack,
			0.0f,
			origin,
			scaleBack,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			str,
			posFront,
			colorFront,
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"QUIT";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 60.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 60.0f * windowHeightRatio + 3.0f, 0, 0);
		colorBack = (AABB(posFront, origin, scaleFront)) ? backColorHighlight : backColorNormal;

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			colorBack,
			0.0f,
			origin,
			scaleBack,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			str,
			posFront,
			colorFront,
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		spriteBatch->End();

		// Reset render states, since sprite batch changes these!
		Config::Context->OMSetBlendState(0, 0, 0xFFFFFFFF);
		Config::Context->OMSetDepthStencilState(0, 0);
	}

	void DrawTooltip() {
		spriteBatch->Begin(SpriteSortMode_Deferred, Renderer::GetInstance()->blendState);

		XMVECTOR frontColorNormal = XMVectorSet(0.3f, 0, 0, skipTooltipTransparency);

		XMVECTOR backColorNormal = XMVectorSet(0, 0, 0, skipTooltipTransparency);

		LPCWSTR str = L"Press LMB to view next page. Press SPACE to skip.";

		XMVECTOR startLen = font->MeasureString(str);

		XMVECTOR origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		XMVECTOR posFront = XMVectorSet(windowCenter.x, windowCenter.y + 200.0f * windowHeightRatio, 0, 0);
		XMVECTOR scaleFront = XMVectorSet(0.5f, 0.5f, 0, 0);
		XMVECTOR colorFront = frontColorNormal;

		XMVECTOR posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 200.0f * windowHeightRatio + 3.0f, 0, 0);
		XMVECTOR scaleBack = XMVectorSet(0.501f, 0.501f, 0, 0);
		XMVECTOR colorBack = backColorNormal;

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			colorBack,
			0.0f,
			origin,
			scaleBack,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			str,
			posFront,
			colorFront,
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		spriteBatch->End();

		// Reset render states, since sprite batch changes these!
		Config::Context->OMSetBlendState(0, 0, 0xFFFFFFFF);
		Config::Context->OMSetDepthStencilState(0, 0);
	}

	void Call() {
		if (transparency > 0.0f) DrawMainMenu();
		else if (skipTooltipTransparency > 0.0f) DrawTooltip();
	}
};

class MainMenuManager : public ScriptManager
{
	vector<Entity*> papers;
	unsigned int numPapers = 0;
	unsigned int paperCounter = 0;
	//bool lerpingPaperSide = false;
	bool lerpingPaperBottom = false;
	bool lerpingPaperCamera = false;
	//XMFLOAT3 paperSidePos = XMFLOAT3(0.4f, 0.3f, 0.2f);
	XMFLOAT3 paperBottomPos = XMFLOAT3(0.8f, 0.0f, 0.4f);
	XMFLOAT3 paperCameraPos;
	XMFLOAT3 originalPaperPos;
	float paperLerpSpd = 0.0f;
	float paperYOffset = 0.01f;
	XMFLOAT3 originalPaperDir;
	XMFLOAT3 originalPaperRight;
	bool papersStarted = false;
	bool skipped = false;

	Mouse* mouse = nullptr;

	LightHandler* lights = nullptr;

	LightContainer* hearthLight = nullptr;
	LightContainer* hearthPtLight = nullptr;

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

	double timeOffsetsRotation[5] = {0.0,5.0,5.0,5.0,3.0};
	double timeOffsetPos = 6.5;

	XMFLOAT3 rotations[5] = {XMFLOAT3(0.64f,1.1108f,0),XMFLOAT3(0,g_XMHalfPi.f[0],0),XMFLOAT3(-0.24f,0.800797f, 0),XMFLOAT3(-0.12f,2.5908f, 0),XMFLOAT3(0,g_XMPi.f[0],0)};
	int rotCounter = 0;
	double rotTimeStamp = 0.0;
	bool nearRot = false;
	float rotSpd = 0.0f;
	XMFLOAT3 positions[2] = { XMFLOAT3(-6.36816f, 3.55001f, -1.79395f),XMFLOAT3(0.4f,4.95f,-3.0f) };//,XMFLOAT3(-5.83396f, 4.09953f, -1.79395f),XMFLOAT3(-3.66047f, 4.66161f, -1.79395f)
	int posCounter = 0;
	float posSpd = 0.5f;

	bool tooltipOn = false;
	bool tooltipClosing = false;

	~MainMenuManager();

	void Init();

	void Update();
};

