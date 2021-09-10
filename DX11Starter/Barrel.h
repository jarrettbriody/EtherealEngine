#pragma once
#include "ScriptManager.h"

using namespace std;

struct Vec3
{
	float x, y, z;

	Vec3 operator+ (const Vec3& other) {
		return { x + other.x, y + other.y, z + other.z };
	}

	Vec3 operator* (const float& scalar) {
		return { x * scalar, y * scalar, z * scalar };
	}
};
struct Quaternion
{
	float x, y, z, w;
};
struct AABB
{
	Vec3 min, max;
};

struct Plane {
	float a, b, c, d;
};

struct SFrustum
{
	float nearPlaneDist;
	float farPlaneDist;
	XMFLOAT3 lookAt;
	float fovY;
	float aspectRatio;

	/*
	where:
	0: near plane
	1: far plane
	2: bottom plane
	3: top plane
	4: left plane
	5: right plane
	*/
	Plane planes[12];

	void CalcFrustumPlanes() {
		float halfFovY = fovY / 2;
		float halfFovX = halfFovY * aspectRatio;

		float cosRatioY = cosf(halfFovY);
		float sinRatioY = sinf(halfFovY);
		float cosRatioX = cosf(halfFovX);
		float sinRatioX = sinf(halfFovX);

		planes[0] = { 0, 0, 1, -nearPlaneDist };
		planes[1] = { 0, 0, -1, farPlaneDist };
		planes[2] = { 0, cosRatioY, sinRatioY, 0 };
		planes[3] = { 0, -cosRatioY, sinRatioY, 0 };
		planes[4] = { cosRatioX, 0, sinRatioX, 0 };
		planes[5] = { -cosRatioX, 0, sinRatioX, 0 };

		planes[6] = { 0, 0, 1, -nearPlaneDist };
		planes[7] = { 0, 0, -1, farPlaneDist };
		planes[8] = { 0, sinRatioY, cosRatioY, 0 };
		planes[9] = { 0, -sinRatioY, cosRatioY, 0 };
		planes[10] = { -sinRatioX, 0, cosRatioX, 0 };
		planes[11] = { sinRatioX, 0, cosRatioX, 0 };
	}
};

struct TestUICallback : Utility::Callback {
	DirectX::SpriteFont* font;
	DirectX::SpriteBatch* spriteBatch;
	string playerPos;

	void DrawPlayerPos() {
		font->DrawString(
			spriteBatch,
			Utility::StringToWideString(playerPos),
			XMVectorSet(0, 0, 0, 0),
			XMVectorSet(1, 1, 1, 1),
			0.0f,
			XMVectorSet(0, 0, 0, 0),
			XMVectorSet(1.0f, 1.0f, 0, 0),
			SpriteEffects::SpriteEffects_None,
			0.0f
		);
	}

	void Call()
	{
		spriteBatch->Begin(SpriteSortMode_Deferred, Renderer::GetInstance()->blendState);

		DrawPlayerPos();

		spriteBatch->End();

		// Reset render states, since sprite batch changes these!
		Config::Context->OMSetBlendState(0, 0, 0xFFFFFFFF);
		Config::Context->OMSetDepthStencilState(0, 0);
	}
};

class TestScript : public ScriptManager
{
	map<string, Entity*>* eMap = ScriptManager::sceneEntitiesMap;

	string test = "Scope";

	XMFLOAT4X4 mat;

	TestUICallback FPSCtrlUICb;

	SFrustum frustum;

	void Init();

	void Update();

	void OnCollision(btCollisionObject* other);

	bool IsVisible();
};

