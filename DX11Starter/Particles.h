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
	float rotationRadians;
	float angularVelocity;
	XMFLOAT2 padding;
};

struct ParticleColor {
	XMFLOAT4 color; //color of the particle in rgba 0.0f-1.0f values
	float weight; //weights as percentage change of that color [0.0f-1.0f] where 0.0f will never spawn and 1.0f will spawn 100% of the time
	XMFLOAT3 padding;
};

struct ParticleSort
{
	unsigned int index;
	float distanceSq;
};