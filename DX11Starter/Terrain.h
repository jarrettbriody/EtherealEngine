#pragma once
#include <d3d11.h>
#include "DXCore.h"
#include <DirectXMath.h>
#include "Vertex.h"
#include "Mesh.h"

using namespace DirectX;

class Terrain : public Mesh
{
public:

	//default
	Terrain(
		ID3D11Device* device,
		int* heightmap,
		unsigned int heightmapWidth,
		unsigned int heightmapHeight,
		float xScale=1.f,
		float yScale=1.f,
		float zScale=1.f
	);

	//loading RAW file
	Terrain(
		ID3D11Device* device,
		const char* heightmap,
		unsigned int heightmapWidth,
		unsigned int heightmapHeight,
		float xScale = 1.f,
		float yScale = 1.f,
		float zScale = 1.f
	);

	~Terrain();

	float GetHeightAtPosition(float x, float y);

private:

	void LoadRaw16Bit(
		const char* heightmap,
		unsigned int width,
		unsigned int height,
		float yScale,
		float xScale,
		float zScale,
		Vertex* verts
	);

	void ConvertHeightmapToVertex(
		int* heightmap,
		unsigned int width,
		unsigned int height,
		float yScale,
		float xScale,
		float zScale,
		Vertex* verts
	);

};

