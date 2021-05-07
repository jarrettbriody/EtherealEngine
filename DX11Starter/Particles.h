#pragma once
#include "pch.h"

using namespace DirectX;

struct Particle
{
	XMFLOAT4 color = ONE_VECTOR4;

	float remainingLife = 0.0f;
	XMFLOAT3 position = ZERO_VECTOR3;

	float scale = 1.0f;
	XMFLOAT3 velocity = ZERO_VECTOR3;

	XMFLOAT3 acceleration = ZERO_VECTOR3;
	float rotationRadians = 0.0f;

	float angularVelocity = 0.0f;
	int textureIndex = -1;
	float transparency = 1.0f;
	float originalTransparency = 1.0f;

	float originalRemainingLife = 0.0f;
	int worldMatBaked = 0;
	XMFLOAT2 padding;
};

struct ParticleColor {
	XMFLOAT4 color = ONE_VECTOR4; //color of the particle in rgba 0.0f-1.0f values
	float weight = 1.0f; //weights as percentage chance of that color [0.0f-1.0f] where 0.0f will never spawn and 1.0f will spawn 100% of the time
	XMFLOAT3 padding;
};

struct ParticleTextureToGPU {
	int index = 0;
	float weight = 1.0f;
	float transparency = 1.0f;
	float padding;
};

struct ParticleTexture {
	ID3D11Texture2D* texture = nullptr;
	float weight = 1.0f; //weights as percentage chance of that texture [0.0f-1.0f] where 0.0f will never spawn and 1.0f will spawn 100% of the time
	float transparency = 1.0f;
};

struct ParticleVertex {
	XMFLOAT3 Position = ZERO_VECTOR3;
	XMFLOAT2 UV = ZERO_VECTOR2;
	XMFLOAT4 Color = ONE_VECTOR4;
	float Scale = 1.0f;
	float RotationRadians = 0.0f;
	int ID = 0;
	int TextureIndex = 0;
	float Transparency = 1.0f;
	int WorldMatBaked = 0;
};

struct ParticleDrawInfo
{
	unsigned int index = 0;
	float distanceSq = 0.0f;
};