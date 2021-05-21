#pragma once
#include "ScriptManager.h"
#include "NavmeshHandler.h"
#include "AudioManager.h"
#include "SpriteFont.h"
#include "SpriteBatch.h"
//#include "FPSController.h"

enum class GameState
{
	Paused, Gameplay, GameOver, Victory
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
	float otherTransparency = 1.0f;
	LPCWSTR tooltip;

	GameState* gs;

	bool AABB(XMVECTOR position, XMVECTOR origin, XMVECTOR scale) {
		XMFLOAT2 start(position.m128_f32[0] - origin.m128_f32[0] * scale.m128_f32[0], position.m128_f32[1] - origin.m128_f32[1] * scale.m128_f32[1]);
		XMFLOAT2 end(position.m128_f32[0] + origin.m128_f32[0] * scale.m128_f32[0], position.m128_f32[1] + origin.m128_f32[1] * scale.m128_f32[1]);
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(Config::hWnd, &mousePos);
		return mousePos.x >= start.x && mousePos.x <= end.x && mousePos.y >= start.y && mousePos.y <= end.y;
	}

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

	void DrawPauseMenu() {
		Mouse* EEMouse = Mouse::GetInstance();

		XMVECTOR frontColorNormal = XMVectorSet(0, 0, 0, otherTransparency);
		XMVECTOR frontColorHighlight = XMVectorSet(0, 0, 0, otherTransparency - 0.4f);

		XMVECTOR backColorNormal = XMVectorSet(0.2f, 0, 0, otherTransparency);
		XMVECTOR backColorHighlight = XMVectorSet(0.2f, 0, 0, otherTransparency - 0.4f);

		LPCWSTR str = L"PAUSED";

		XMVECTOR startLen = font->MeasureString(str);

		XMVECTOR origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		XMVECTOR posFront = XMVectorSet(windowCenter.x, windowCenter.y - 275.0f * windowHeightRatio, 0, 0);
		XMVECTOR scaleFront = XMVectorSet(0.75f * windowWidthRatio, 0.75f * windowHeightRatio, 0, 0);
		XMVECTOR colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		XMVECTOR posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 275.0f * windowHeightRatio + 3.0f, 0, 0);
		XMVECTOR scaleBack = XMVectorSet(0.751f * windowWidthRatio, 0.751f * windowHeightRatio, 0, 0);
		XMVECTOR colorBack = (AABB(posFront, origin, scaleFront)) ? backColorHighlight : backColorNormal;

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
			0.0f,
			origin,
			XMVectorSet(1.001f * windowWidthRatio, 1.001f * windowHeightRatio, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			str,
			posFront,
			XMVectorSet(0.6f, 0, 0, 1.0f),
			0.0f,
			origin,
			XMVectorSet(1.00f * windowWidthRatio, 1.00f * windowHeightRatio, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"CONTINUE";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 100.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 100.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront)) {
			if (EEMouse->OnLMBDown()) {
				Config::UpdatePaused = false;
				*gs = GameState::Gameplay;

				Config::ShowCursor = false;
				while (ShowCursor(Config::ShowCursor) >= 0);
			}
		}

		str = L"RESTART LEVEL";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 30.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 30.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront)) {
			if (EEMouse->OnLMBDown()) {
				Config::FMODResult = Config::MusicGroup->stop();
				AudioManager::FMODErrorCheck(Config::FMODResult);

				SceneLoader::GetInstance()->SetModelPath("../../Assets/Models/Kamchatka/");
				SceneLoader::GetInstance()->LoadScene("Kamchatka");

				Config::UpdatePaused = false;
			}
		}

		str = L"QUIT TO MAIN MENU";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 40.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 40.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront)) {
			//if (EEMouse->OnLMBDown()) {
			//	Config::FMODResult = Config::MusicGroup->stop();
			//	AudioManager::FMODErrorCheck(Config::FMODResult);
			//
			//	SceneLoader::GetInstance()->SetModelPath("../../Assets/Models/MainMenu/");
			//	SceneLoader::GetInstance()->LoadScene("MainMenu");
			//
			//	Config::UpdatePaused = false;
			//}
		}

		str = L"QUIT TO DESKTOP";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 110.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 110.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront)) {
			if (EEMouse->OnLMBDown()) {
				Config::SwapChain->SetFullscreenState(false, NULL);
				PostMessage(Config::hWnd, WM_CLOSE, NULL, NULL);
			}
		}
	}

	void DrawGameOver() {
		Mouse* EEMouse = Mouse::GetInstance();

		XMVECTOR frontColorNormal = XMVectorSet(0, 0, 0, otherTransparency);
		XMVECTOR frontColorHighlight = XMVectorSet(0, 0, 0, otherTransparency - 0.4f);

		XMVECTOR backColorNormal = XMVectorSet(0.2f, 0, 0, otherTransparency);
		XMVECTOR backColorHighlight = XMVectorSet(0.2f, 0, 0, otherTransparency - 0.4f);

		LPCWSTR str = L"GAME OVER";

		XMVECTOR startLen = font->MeasureString(str);

		XMVECTOR origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		XMVECTOR posFront = XMVectorSet(windowCenter.x, windowCenter.y - 275.0f * windowHeightRatio, 0, 0);
		XMVECTOR scaleFront = XMVectorSet(0.75f * windowWidthRatio, 0.75f * windowHeightRatio, 0, 0);
		XMVECTOR colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		XMVECTOR posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 275.0f * windowHeightRatio + 3.0f, 0, 0);
		XMVECTOR scaleBack = XMVectorSet(0.751f * windowWidthRatio, 0.751f * windowHeightRatio, 0, 0);
		XMVECTOR colorBack = (AABB(posFront, origin, scaleFront)) ? backColorHighlight : backColorNormal;

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
			0.0f,
			origin,
			XMVectorSet(1.001f * windowWidthRatio, 1.001f * windowHeightRatio, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			str,
			posFront,
			XMVectorSet(0.6f, 0, 0, 1.0f),
			0.0f,
			origin,
			XMVectorSet(1.00f * windowWidthRatio, 1.00f * windowHeightRatio, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"RESTART LEVEL";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 100.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 100.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront)) {
			if (EEMouse->OnLMBDown()) {
				Config::FMODResult = Config::MusicGroup->stop();
				AudioManager::FMODErrorCheck(Config::FMODResult);

				SceneLoader::GetInstance()->SetModelPath("../../Assets/Models/Kamchatka/");
				SceneLoader::GetInstance()->LoadScene("Kamchatka");

				Config::UpdatePaused = false;
			}
		}

		str = L"QUIT TO MAIN MENU";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 0.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 0.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront)) {
			//if (EEMouse->OnLMBDown()) {
			//	Config::FMODResult = Config::MusicGroup->stop();
			//	AudioManager::FMODErrorCheck(Config::FMODResult);
			//
			//	SceneLoader::GetInstance()->SetModelPath("../../Assets/Models/MainMenu/");
			//	SceneLoader::GetInstance()->LoadScene("MainMenu");
			//
			//	Config::UpdatePaused = false;
			//}
		}

		str = L"QUIT TO DESKTOP";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 100.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 100.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront)) {
			if (EEMouse->OnLMBDown()) {
				Config::SwapChain->SetFullscreenState(false, NULL);
				PostMessage(Config::hWnd, WM_CLOSE, NULL, NULL);
			}
		}
	}

	void DrawVictory() {
		Mouse* EEMouse = Mouse::GetInstance();

		XMVECTOR frontColorNormal = XMVectorSet(0, 0, 0, otherTransparency);
		XMVECTOR frontColorHighlight = XMVectorSet(0, 0, 0, otherTransparency - 0.4f);

		XMVECTOR backColorNormal = XMVectorSet(0.2f, 0, 0, otherTransparency);
		XMVECTOR backColorHighlight = XMVectorSet(0.2f, 0, 0, otherTransparency - 0.4f);

		LPCWSTR str = L"VICTORY";

		XMVECTOR startLen = font->MeasureString(str);

		XMVECTOR origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		XMVECTOR posFront = XMVectorSet(windowCenter.x, windowCenter.y - 275.0f * windowHeightRatio, 0, 0);
		XMVECTOR scaleFront = XMVectorSet(0.75f * windowWidthRatio, 0.75f * windowHeightRatio, 0, 0);
		XMVECTOR colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		XMVECTOR posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 275.0f * windowHeightRatio + 3.0f, 0, 0);
		XMVECTOR scaleBack = XMVectorSet(0.751f * windowWidthRatio, 0.751f * windowHeightRatio, 0, 0);
		XMVECTOR colorBack = (AABB(posFront, origin, scaleFront)) ? backColorHighlight : backColorNormal;

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
			0.0f,
			origin,
			XMVectorSet(1.001f * windowWidthRatio, 1.001f * windowHeightRatio, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			str,
			posFront,
			XMVectorSet(0.6f, 0, 0, 1.0f),
			0.0f,
			origin,
			XMVectorSet(1.00f * windowWidthRatio, 1.00f * windowHeightRatio, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"RESTART LEVEL";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 100.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 100.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront)) {
			if (EEMouse->OnLMBDown()) {
				Config::FMODResult = Config::MusicGroup->stop();
				AudioManager::FMODErrorCheck(Config::FMODResult);

				SceneLoader::GetInstance()->SetModelPath("../../Assets/Models/Kamchatka/");
				SceneLoader::GetInstance()->LoadScene("Kamchatka");

				Config::UpdatePaused = false;
			}
		}

		str = L"QUIT TO MAIN MENU";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 0.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 0.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront)) {
			//if (EEMouse->OnLMBDown()) {
			//	Config::FMODResult = Config::MusicGroup->stop();
			//	AudioManager::FMODErrorCheck(Config::FMODResult);
			//
			//	SceneLoader::GetInstance()->SetModelPath("../../Assets/Models/MainMenu/");
			//	SceneLoader::GetInstance()->LoadScene("MainMenu");
			//
			//	Config::UpdatePaused = false;
			//}
		}

		str = L"QUIT TO DESKTOP";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 100.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 100.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront)) {
			if (EEMouse->OnLMBDown()) {
				Config::SwapChain->SetFullscreenState(false, NULL);
				PostMessage(Config::hWnd, WM_CLOSE, NULL, NULL);
			}
		}
	}

	void Call()
	{
		spriteBatch->Begin(SpriteSortMode_Deferred, Renderer::GetInstance()->blendState);

		switch (*gs)
		{
		case GameState::Paused:
		{
			DrawPauseMenu();
			if (Keyboard::GetInstance()->OnKeyDown(VK_ESCAPE)) {
				Config::UpdatePaused = false;
				*gs = GameState::Gameplay;
				Config::ShowCursor = false;
				while (ShowCursor(Config::ShowCursor) >= 0);
			}
			break;
		}
		case GameState::Gameplay:
		{
			DrawCrosshair();
			if (transparency > 0.0f) DrawInGameUI();
			break;
		}
		case GameState::GameOver:
		{
			DrawGameOver();
			break;
		}
		case GameState::Victory:
		{
			DrawVictory();
			break;
		}
		default:
			break;
		}


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

	unsigned long long clock_start = 0;

	ID3D11Resource* crosshairResource;
	ID3D11Texture2D* crosshairTexture;

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

	GameState gs;

	void DecrementEnemiesAlive();
};

