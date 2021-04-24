#pragma once
#include "pch.h"

using namespace DirectX;

struct Particle
{
	XMFLOAT4 color;

	float remainingLife;
	XMFLOAT3 position;

	float scale;
	XMFLOAT3 velocity;

	XMFLOAT3 acceleration;
	float rotationRadians;

	float angularVelocity;
	int textureIndex = -1;
	float transparency = 1.0f;
	float originalTransparency = 1.0f;

	float originalRemainingLife;
	int worldMatBaked = 0;
	XMFLOAT2 padding;
};

struct ParticleColor {
	XMFLOAT4 color; //color of the particle in rgba 0.0f-1.0f values
	float weight; //weights as percentage chance of that color [0.0f-1.0f] where 0.0f will never spawn and 1.0f will spawn 100% of the time
	XMFLOAT3 padding;
};

struct ParticleTextureToGPU {
	int index;
	float weight;
	float transparency = 1.0f;
	float padding;
};

struct ParticleTexture {
	ID3D11Texture2D* texture;
	float weight; //weights as percentage chance of that texture [0.0f-1.0f] where 0.0f will never spawn and 1.0f will spawn 100% of the time
	float transparency = 1.0f;
};

struct ParticleVertex {
	XMFLOAT3 Position;
	XMFLOAT2 UV;
	XMFLOAT4 Color;
	float Scale;
	float RotationRadians;
	int ID;
	int TextureIndex;
	float Transparency;
	//int WorldMatBaked;
};

struct ParticleDrawInfo
{
	unsigned int index;
	float distanceSq;
};