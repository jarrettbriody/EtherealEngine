#pragma once
#include "pch.h"
#include "Terrain.h"
#include "DXCore.h"
#include "Vertex.h"


class Water
{
public:
	Terrain* terrain;

	void Update();

	Water(float speed,
		ID3D11Device* device,
		unsigned int width,
		unsigned int height,
		float xScale,
		float yScale,
		float zScale,
		SimplePixelShader* waterPS);
	~Water();

	void SetOffsets(float x1, float y1, float x2, float y2);

private:

	float speed;

	void Setup(ID3D11Device*);

	float xScale;
	float yScale;
	float zScale;
	unsigned int width;
	unsigned int height;

	float normalOffsetX1 = 0.f;
	float normalOffsetY1 = 0.f;
	float normalOffsetX2 = 0.f;
	float normalOffsetY2 = 0.f;

	float offsetX1;
	float offsetY1;

	float offsetX2;
	float offsetY2;

	SimplePixelShader* waterPS;


};

