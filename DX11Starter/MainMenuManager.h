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

enum class MainMenuState {
	MAIN,
	OPTIONS,
	CREDITS,
};

enum class OptionsButton {
	NONE,
	BRIGHTNESS,
	MUSIC_VOL,
	SFX_VOL,
};

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
	Keyboard* EEKeyboard = nullptr;
	MainMenuState state = MainMenuState::MAIN;
	OptionsButton optionsButton = OptionsButton::NONE;

	float skipTooltipTransparency = 0.0f;

	FMOD::Channel* music[3];

	bool AABB(XMVECTOR position, XMVECTOR origin, XMVECTOR scale) {
		XMFLOAT2 start(position.m128_f32[0] - origin.m128_f32[0] * scale.m128_f32[0], position.m128_f32[1] - origin.m128_f32[1] * scale.m128_f32[1]);
		XMFLOAT2 end(position.m128_f32[0] + origin.m128_f32[0] * scale.m128_f32[0], position.m128_f32[1] + origin.m128_f32[1] * scale.m128_f32[1]);
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(Config::hWnd, &mousePos);
		return mousePos.x >= start.x && mousePos.x <= end.x && mousePos.y >= start.y && mousePos.y <= end.y;
	}

	void DrawMainMenu() {
		XMVECTOR titleLen = font->MeasureString(L"A Demon Killed My Babushka");
		font->DrawString(
			spriteBatch,
			L"A Demon Killed My Babushka",
			XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 275.0f * windowHeightRatio + 3.0f, 0, 0),
			XMVectorSet(0, 0, 0, transparency),
			0.0f,
			XMVectorSet(titleLen.m128_f32[0] / 2.0f, titleLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(1.001f * windowWidthRatio, 1.001f * windowHeightRatio, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
		font->DrawString(
			spriteBatch,
			L"A Demon Killed My Babushka",
			XMVectorSet(windowCenter.x, windowCenter.y - 275.0f * windowHeightRatio, 0, 0),
			XMVectorSet(0.6f, 0, 0, transparency),
			0.0f,
			XMVectorSet(titleLen.m128_f32[0] / 2.0f, titleLen.m128_f32[1] / 2.0f, 0, 0),
			XMVectorSet(1.0f * windowWidthRatio, 1.0f * windowHeightRatio, 0, 0),
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

		XMVECTOR posFront = XMVectorSet(windowCenter.x, windowCenter.y - 100.0f * windowHeightRatio, 0, 0);
		XMVECTOR scaleFront = XMVectorSet(0.75f * windowWidthRatio, 0.75f * windowHeightRatio, 0, 0);
		XMVECTOR colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		XMVECTOR posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 100.0f * windowHeightRatio + 3.0f, 0, 0);
		XMVECTOR scaleBack = XMVectorSet(0.751f * windowWidthRatio, 0.751f * windowHeightRatio, 0, 0);
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
			if (EEMouse->OnLMBDown()) {
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

		if (AABB(posFront, origin, scaleFront) && transparency >= 0.3f) {
			if (EEMouse->OnLMBDown()) {
				state = MainMenuState::OPTIONS;
			}
		}

		str = L"CREDITS";

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

		if (AABB(posFront, origin, scaleFront) && transparency >= 0.3f) {
			if (EEMouse->OnLMBDown()) {
				state = MainMenuState::CREDITS;
			}
		}

		str = L"QUIT";

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

		if (AABB(posFront, origin, scaleFront) && transparency >= 0.3f) {
			if (EEMouse->OnLMBDown()) {
				Config::SwapChain->SetFullscreenState(false, NULL);
				PostMessage(Config::hWnd, WM_CLOSE, NULL, NULL);
			}
		}
	}

	void DrawTooltip() {
		XMVECTOR frontColorNormal = XMVectorSet(0.3f, 0, 0, skipTooltipTransparency);

		XMVECTOR backColorNormal = XMVectorSet(0, 0, 0, skipTooltipTransparency);

		LPCWSTR str = L"Press LMB to view next page. Press SPACE to skip.";

		XMVECTOR startLen = font->MeasureString(str);

		XMVECTOR origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		XMVECTOR posFront = XMVectorSet(windowCenter.x, windowCenter.y + 350.0f * windowHeightRatio, 0, 0);
		XMVECTOR scaleFront = XMVectorSet(0.5f, 0.5f, 0, 0);
		XMVECTOR colorFront = frontColorNormal;

		XMVECTOR posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 350.0f * windowHeightRatio + 3.0f, 0, 0);
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
	}

	void DrawOptionsMenu() {
		XMVECTOR frontColorNormal = XMVectorSet(0, 0, 0, otherTransparency);
		XMVECTOR frontColorHighlight = XMVectorSet(0, 0, 0, otherTransparency - 0.4f);

		XMVECTOR backColorNormal = XMVectorSet(0.2f, 0, 0, otherTransparency);
		XMVECTOR backColorHighlight = XMVectorSet(0.2f, 0, 0, otherTransparency - 0.4f);

		LPCWSTR str = L"OPTIONS";

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

		XMVECTOR optionSelectedColor = XMVectorSet(0, 0.5f, 0, 1);

		str = Utility::StringToWideString("BRIGHTNESS (UP/DOWN): " + to_string(Config::SceneBrightnessMult));

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 100.0f * windowHeightRatio, 0, 0);
		colorFront = (optionsButton == OptionsButton::BRIGHTNESS) ? optionSelectedColor : (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

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

		if (AABB(posFront, origin, scaleFront) && transparency >= 0.3f) {
			if (EEMouse->OnLMBDown()) {
				optionsButton = OptionsButton::BRIGHTNESS;
			}
		}

		str = Utility::StringToWideString("MUSIC VOLUME (UP/DOWN): " + to_string(Config::MusicVolume));

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 30.0f * windowHeightRatio, 0, 0);
		colorFront = (optionsButton == OptionsButton::MUSIC_VOL) ? optionSelectedColor : (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

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

		if (AABB(posFront, origin, scaleFront) && transparency >= 0.3f) {
			if (EEMouse->OnLMBDown()) {
				optionsButton = OptionsButton::MUSIC_VOL;
			}
		}

		str = Utility::StringToWideString("SFX VOLUME (UP/DOWN): " + to_string(Config::SFXVolume));

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 40.0f * windowHeightRatio, 0, 0);
		colorFront = (optionsButton == OptionsButton::SFX_VOL) ? optionSelectedColor : (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

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

		if (AABB(posFront, origin, scaleFront) && transparency >= 0.3f) {
			if (EEMouse->OnLMBDown()) {
				optionsButton = OptionsButton::SFX_VOL;
			}
		}

		str = L"BACK";

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

		if (AABB(posFront, origin, scaleFront) && transparency >= 0.3f) {
			if (EEMouse->OnLMBDown()) {
				state = MainMenuState::MAIN;
			}
		}

		switch (optionsButton)
		{
		case OptionsButton::NONE:
			break;
		case OptionsButton::BRIGHTNESS:
		{
			if (EEKeyboard->OnKeyDown(0x26)) {
				Config::SceneBrightnessMult += 0.05f;
				if (Config::SceneBrightnessMult > 2.0f) Config::SceneBrightnessMult = 2.0f;
			}
			if (EEKeyboard->OnKeyDown(0x28)) {
				Config::SceneBrightnessMult -= 0.05f;
				if (Config::SceneBrightnessMult < 0.1f) Config::SceneBrightnessMult = 0.1f;
			}
			break;
		}
		case OptionsButton::MUSIC_VOL:
		{
			if (EEKeyboard->OnKeyDown(0x26)) {
				Config::MusicVolume += 0.05f;
				if (Config::MusicVolume > 1.0f) Config::MusicVolume = 1.0f;

				for (size_t i = 0; i < 3; i++)
				{
					Config::FMODResult = music[i]->setVolume(Config::MusicVolume);
					AudioManager::FMODErrorCheck(Config::FMODResult);
				}
			}
			if (EEKeyboard->OnKeyDown(0x28)) {
				Config::MusicVolume -= 0.05f;
				if (Config::MusicVolume < 0.0f) Config::MusicVolume = 0.0f;

				for (size_t i = 0; i < 3; i++)
				{
					Config::FMODResult = music[i]->setVolume(Config::MusicVolume);
					AudioManager::FMODErrorCheck(Config::FMODResult);
				}
			}
			break;
		}
		case OptionsButton::SFX_VOL:
		{
			if (EEKeyboard->OnKeyDown(0x26)) {
				Config::SFXVolume += 0.05f;
				if (Config::SFXVolume > 2.0f) Config::SFXVolume = 2.0f;
			}
			if (EEKeyboard->OnKeyDown(0x28)) {
				Config::SFXVolume -= 0.05f;
				if (Config::SFXVolume < 0.0f) Config::SFXVolume = 0.0f;
			}
			break;
		}
		default:
			break;
		}
	}

	void DrawCreditsMenu() {
		XMVECTOR frontColorNormal = XMVectorSet(0, 0, 0, otherTransparency);
		XMVECTOR frontColorHighlight = XMVectorSet(0, 0, 0, otherTransparency - 0.4f);

		XMVECTOR backColorNormal = XMVectorSet(0.2f, 0, 0, otherTransparency);
		XMVECTOR backColorHighlight = XMVectorSet(0.2f, 0, 0, otherTransparency - 0.4f);

		LPCWSTR str = L"CREDITS";

		XMVECTOR startLen = font->MeasureString(str);

		XMVECTOR origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		XMVECTOR posFront = XMVectorSet(windowCenter.x, windowCenter.y - 275.0f * windowHeightRatio, 0, 0);
		XMVECTOR scaleFront = XMVectorSet(0.5f * windowWidthRatio, 0.5f * windowHeightRatio, 0, 0);
		XMVECTOR colorFront = frontColorNormal;

		XMVECTOR posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 275.0f * windowHeightRatio + 3.0f, 0, 0);
		XMVECTOR scaleBack = XMVectorSet(0.501f * windowWidthRatio, 0.501f * windowHeightRatio, 0, 0);
		XMVECTOR colorBack = backColorNormal;

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

		str = L"Niko Bazos | Project Lead, Gameplay & Engine Developer";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 200.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 200.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"Jarrett Briody | Lead Engine Developer, Gameplay Developer";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 160.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 160.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"Robert MacLeod | Engine Developer, AI Programmer";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 120.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 120.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"Israel Anthony | Gameplay Programmer";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 80.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 80.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"Coehl Gleckner | Lead Designer";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 40.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 40.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"Nathan Gibson | Sound Designer";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y - 0.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y - 0.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"Madilyn Chandler | 3D Game Artist, Concept Artist";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 40.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 40.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"Nathalie Simon | 2D Game Artist, Concept Artist";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 80.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 80.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"Alex Brandt | 2D Game Artist, Concept Artist";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 120.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 120.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"Nick Passenese | Concept Artist";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 160.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 160.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		str = L"Elizabeth Bogart | Copy Editor";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 200.0f * windowHeightRatio, 0, 0);

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 200.0f * windowHeightRatio + 3.0f, 0, 0);

		font->DrawString(
			spriteBatch,
			str,
			posBack,
			frontColorNormal,
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
			XMVectorSet(0.3f, 0, 0, 1.0f),
			0.0f,
			origin,
			scaleFront,
			SpriteEffects::SpriteEffects_None,
			0.0f
		);

		scaleFront = XMVectorSet(0.75f * windowWidthRatio, 0.75f * windowHeightRatio, 0, 0);
		scaleBack = XMVectorSet(0.751f * windowWidthRatio, 0.751f * windowHeightRatio, 0, 0);

		str = L"BACK";

		startLen = font->MeasureString(str);

		origin = XMVectorSet(startLen.m128_f32[0] / 2.0f, startLen.m128_f32[1] / 2.0f, 0, 0);

		posFront = XMVectorSet(windowCenter.x, windowCenter.y + 275.0f * windowHeightRatio, 0, 0);
		colorFront = (AABB(posFront, origin, scaleFront)) ? frontColorHighlight : frontColorNormal;

		posBack = XMVectorSet(windowCenter.x + 3.0f, windowCenter.y + 275.0f * windowHeightRatio + 3.0f, 0, 0);
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

		if (AABB(posFront, origin, scaleFront) && transparency >= 0.3f) {
			if (EEMouse->OnLMBDown()) {
				state = MainMenuState::MAIN;
			}
		}
	}

	void Call() {
		spriteBatch->Begin(SpriteSortMode_Deferred, Renderer::GetInstance()->blendState);

		if (transparency > 0.0f) {
			switch (state)
			{
			case MainMenuState::MAIN:
				DrawMainMenu();
				break;
			case MainMenuState::OPTIONS:
				DrawOptionsMenu();
				break;
			case MainMenuState::CREDITS:
				DrawCreditsMenu();
				break;
			default:
				break;
			}
		}
		else if (skipTooltipTransparency > 0.0f) DrawTooltip();

		spriteBatch->End();

		// Reset render states, since sprite batch changes these!
		Config::Context->OMSetBlendState(0, 0, 0xFFFFFFFF);
		Config::Context->OMSetDepthStencilState(0, 0);
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
	bool paperAtCam = false;
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
	bool finishedAnimation = false;

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

	bool soundPlayed = false;

	~MainMenuManager();

	void Init();

	void Update();
};

