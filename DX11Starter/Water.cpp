#include "pch.h"
#include "Water.h"

Water::Water(float speed, 
	ID3D11Device* device,
	unsigned int width,
	unsigned int height,
	float xScale,
	float yScale,
	float zScale,
	SimplePixelShader* waterPS)
{
	this->speed = speed;

	this->height = height;
	this->width = width;

	this->xScale = xScale;
	this->yScale = yScale;
	this->zScale = zScale;

	this->waterPS = waterPS;

	Setup(device);
}

Water::~Water()
{

}

void Water::Setup(ID3D11Device* device)
{
	//generate heightmap
	int size = width * height;
	int* heightmap = new int[size];

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			heightmap[x + (width * y)] = 0;
		}
	}


	terrain = new Terrain(device, heightmap, width, height, xScale, yScale, zScale);
}

void Water::SetOffsets(float x1, float y1, float x2, float y2)
{
	offsetX1 = x1;
	offsetX2 = x2;
	offsetY1 = y1;
	offsetY2 = y2;
}

void Water::Update()
{
	normalOffsetX1 += offsetX1 * speed;
	normalOffsetX2 += offsetX2 * speed;
	normalOffsetY1 += offsetY1 * speed;
	normalOffsetY2 += offsetY2 * speed;

	waterPS->SetFloat("normalOffsetX1", normalOffsetX1);
	waterPS->SetFloat("normalOffsetX2", normalOffsetX2);
	waterPS->SetFloat("normalOffsetY1", normalOffsetY1);
	waterPS->SetFloat("normalOffsetY2", normalOffsetY2);


}

