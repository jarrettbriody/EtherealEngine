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
	XMFLOAT3 padding;
};

struct ParticleColor {
	XMFLOAT4 color; //color of the particle in rgba 0.0f-1.0f values
	float weight; //weights as percentage change of that color [0.0f-1.0f] where 0.0f will never spawn and 1.0f will spawn 100% of the time
	XMFLOAT3 padding;
};

struct ParticleVertex {
	XMFLOAT3 Position;
	XMFLOAT2 UV;
	XMFLOAT4 Color;
	float Scale;
	int ID;
};

struct ParticleDrawInfo
{
	unsigned int index;
	float distanceSq;
};