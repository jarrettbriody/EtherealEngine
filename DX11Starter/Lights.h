#pragma once
#include "pch.h"
#include "EEString.h"

#define LIGHT_TYPE_DIR 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

using namespace DirectX;

struct Light {
	int Type;
	XMFLOAT3 Direction;
	float Range;
	XMFLOAT3 Position;
	float Intensity;
	XMFLOAT3 Color;
	float SpotFalloff;
	XMFLOAT3 Padding;
};

struct LightShaders {
	SimpleVertexShader* VS = nullptr;
	SimplePixelShader* PS = nullptr;
};

struct LightContainer {
	Light light;
	EEString<64> lightName = "";
	EEString<64> parentName = ""; //Optional
	XMFLOAT4X4* parentWorld = nullptr; //Optional
	XMFLOAT4X4 world;
	//LightShaders shaders; //Optional
};