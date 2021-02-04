#include "Terrain.h"


Terrain::Terrain(ID3D11Device* device,
	int* heightmap,
	unsigned int heightmapWidth,
	unsigned int heightmapHeight,
	float xScale,
	float yScale,
	float zScale) : Mesh()
{
	unsigned int numVerticies = heightmapWidth * heightmapHeight;
	unsigned int numIndicies = (heightmapWidth - 1) * (heightmapHeight - 1) * 6; // - 1 for each because we start at 0, * 6 for triangle data

	Vertex* verts = new Vertex[numVerticies];

	ConvertHeightmapToVertex(heightmap, heightmapWidth, heightmapHeight, yScale, xScale, zScale, verts);

	unsigned int* indices = new unsigned int[numIndicies];
	std::vector<XMFLOAT3> triangleNormals;

	int indexCount = 0;
	for (int z = 0; z < heightmapHeight - 1; z++)
	{
		for (int x = 0; x < heightmapWidth - 1; x++)
		{
			int vertIndex = z * heightmapWidth + x;

			int i0 = vertIndex;
			int i1 = vertIndex + heightmapWidth;
			int i2 = vertIndex + 1 + heightmapWidth;

			int i3 = vertIndex;
			int i4 = vertIndex + 1 + heightmapWidth;
			int i5 = vertIndex + 1;

			indices[indexCount++] = i0;
			indices[indexCount++] = i1;
			indices[indexCount++] = i2;

			indices[indexCount++] = i3;
			indices[indexCount++] = i4;
			indices[indexCount++] = i5;

			XMVECTOR pos0 = XMLoadFloat3(&verts[i0].Position);
			XMVECTOR pos1 = XMLoadFloat3(&verts[i1].Position);
			XMVECTOR pos2 = XMLoadFloat3(&verts[i2].Position);

			XMVECTOR pos3 = XMLoadFloat3(&verts[i3].Position);
			XMVECTOR pos4 = XMLoadFloat3(&verts[i4].Position);
			XMVECTOR pos5 = XMLoadFloat3(&verts[i5].Position);

			XMFLOAT3 normal0;
			XMFLOAT3 normal1;

			XMStoreFloat3(&normal0, XMVector3Normalize(XMVector3Cross(XMVectorSubtract(pos1, pos0), XMVectorSubtract(pos2, pos0))));
			XMStoreFloat3(&normal1, XMVector3Normalize(XMVector3Cross(XMVectorSubtract(pos4, pos3), XMVectorSubtract(pos5, pos3))));

			triangleNormals.push_back(normal0);
			triangleNormals.push_back(normal1);
		}
	}

	for (int z = 0; z < heightmapHeight; z++)
	{
		for (int x = 0; x < heightmapWidth; x++)
		{
			int index = z * heightmapWidth + x;
			int triIndex = index * 2 - (2 * z);
			int triIndexPrevRow = triIndex - (heightmapWidth * 2 - 1);

			int normalCount = 0;
			XMVECTOR normalTotal = XMVectorSet(0, 0, 0, 0);

			if (z > 0 && x > 0)
			{
				normalTotal += XMLoadFloat3(&triangleNormals[triIndexPrevRow - 1]);
				normalTotal += XMLoadFloat3(&triangleNormals[triIndexPrevRow]);

				normalCount += 2;
			}

			if (z > 0 && x < heightmapWidth - 1)
			{
				normalTotal += XMLoadFloat3(&triangleNormals[triIndexPrevRow + 1]);

				normalCount++;
			}

			if (z < heightmapHeight - 1 && x > 0)
			{
				normalTotal += XMLoadFloat3(&triangleNormals[triIndex - 1]);

				normalCount++;
			}

			if (z < heightmapHeight - 1 && x < heightmapWidth - 1)
			{
				normalTotal += XMLoadFloat3(&triangleNormals[triIndex]);
				normalTotal += XMLoadFloat3(&triangleNormals[triIndex + 1]);

				normalCount += 2;
			}

			normalTotal /= normalCount;
			XMStoreFloat3(&verts[index].Normal, normalTotal);
		}
	}

	this->CreateBuffers(verts, numVerticies, indices, numIndicies, device);
	delete[] verts;
	delete[] indices;
}

Terrain::Terrain(ID3D11Device* device,
	const char* heightmap,
	unsigned int heightmapWidth,
	unsigned int heightmapHeight,
	float xScale,
	float yScale,
	float zScale) : Mesh()
{
	unsigned int numVerticies = heightmapWidth * heightmapHeight;
	unsigned int numIndicies = (heightmapWidth - 1) * (heightmapHeight - 1) * 6; // - 1 for each because we start at 0, * 6 for triangle data

	Vertex* verts = new Vertex[numVerticies];

	LoadRaw16Bit(heightmap, heightmapWidth, heightmapHeight, yScale, xScale, zScale, verts);

	unsigned int* indices = new unsigned int[numIndicies];
	std::vector<XMFLOAT3> triangleNormals;

	int indexCount = 0;
	for (int z = 0; z < heightmapHeight - 1; z++)
	{
		for (int x = 0; x < heightmapWidth - 1; x++)
		{
			int vertIndex = z * heightmapWidth + x;

			int i0 = vertIndex;
			int i1 = vertIndex + heightmapWidth;
			int i2 = vertIndex + 1 + heightmapWidth;

			int i3 = vertIndex;
			int i4 = vertIndex + 1 + heightmapWidth;
			int i5 = vertIndex + 1;

			indices[indexCount++] = i0;
			indices[indexCount++] = i1;
			indices[indexCount++] = i2;

			indices[indexCount++] = i3;
			indices[indexCount++] = i4;
			indices[indexCount++] = i5;

			XMVECTOR pos0 = XMLoadFloat3(&verts[i0].Position);
			XMVECTOR pos1 = XMLoadFloat3(&verts[i1].Position);
			XMVECTOR pos2 = XMLoadFloat3(&verts[i2].Position);

			XMVECTOR pos3 = XMLoadFloat3(&verts[i3].Position);
			XMVECTOR pos4 = XMLoadFloat3(&verts[i4].Position);
			XMVECTOR pos5 = XMLoadFloat3(&verts[i5].Position);

			XMFLOAT3 normal0;
			XMFLOAT3 normal1;

			XMStoreFloat3(&normal0, XMVector3Normalize(XMVector3Cross(XMVectorSubtract(pos1, pos0), XMVectorSubtract(pos2, pos0))));
			XMStoreFloat3(&normal1, XMVector3Normalize(XMVector3Cross(XMVectorSubtract(pos4, pos3), XMVectorSubtract(pos5, pos3))));

			triangleNormals.push_back(normal0);
			triangleNormals.push_back(normal1);
		}
	}

	for (int z = 0; z < heightmapHeight; z++)
	{
		for (int x = 0; x < heightmapWidth; x++)
		{
			int index = z * heightmapWidth + x;
			int triIndex = index * 2 - (2 * z);
			int triIndexPrevRow = triIndex - (heightmapWidth * 2 - 1);

			int normalCount = 0;
			XMVECTOR normalTotal = XMVectorSet(0, 0, 0, 0);

			if (z > 0 && x > 0)
			{
				normalTotal += XMLoadFloat3(&triangleNormals[triIndexPrevRow - 1]);
				normalTotal += XMLoadFloat3(&triangleNormals[triIndexPrevRow]);

				normalCount += 2;
			}

			if (z > 0 && x < heightmapWidth - 1)
			{
				normalTotal += XMLoadFloat3(&triangleNormals[triIndexPrevRow + 1]);

				normalCount++;
			}

			if (z < heightmapHeight - 1 && x > 0)
			{
				normalTotal += XMLoadFloat3(&triangleNormals[triIndex - 1]);

				normalCount++;
			}

			if (z < heightmapHeight - 1 && x < heightmapWidth - 1)
			{
				normalTotal += XMLoadFloat3(&triangleNormals[triIndex]);
				normalTotal += XMLoadFloat3(&triangleNormals[triIndex + 1]);

				normalCount += 2;
			}

			normalTotal /= normalCount;
			XMStoreFloat3(&verts[index].Normal, normalTotal);
		}
	}

	this->CreateBuffers(verts, numVerticies, indices, numIndicies, device);
	delete[] verts;
	delete[] indices;
}

Terrain::~Terrain()
{

}

void Terrain::LoadRaw16Bit(const char* heightmap, unsigned int width, unsigned int height, float yScale, float xScale, float zScale, Vertex* verts)
{
	unsigned int numVertices = width * height;

	// Vector to hold heights
	std::vector<unsigned short> heights(numVertices);

	// Open the file (remember to #include <fstream>)
	std::ifstream file;
	file.open(heightmap, std::ios_base::binary);
	if (!file)
		return;

	// Read raw bytes to vector
	file.read((char*)& heights[0], numVertices * 2); // Double the size, since each pixel is 16-bit
	file.close();

	// Create the initial mesh data
	for (int z = 0; z < height; z++)
	{
		for (int x = 0; x < width; x++)
		{
			// This vert index
			int index = z * width + x;

			// Set up this vertex
			verts[index] = {};

			verts[index].Position.x = (x - (width / 2.0f)) * xScale;
			verts[index].Position.y = (heights[index] / 65535.f) * yScale; // 16-bit, so max value is 65535
			verts[index].Position.z = (z - (height / 2.0f)) * zScale;

			verts[index].Normal.x = 0.f;
			verts[index].Normal.y = 1.f;
			verts[index].Normal.z = 0.f;

			verts[index].UV.x = x / (float)width;
			verts[index].UV.y = z / (float)height;
		}
	}
}

void Terrain::ConvertHeightmapToVertex(int* heightmap, unsigned int width, unsigned int height, float yScale, float xScale, float zScale, Vertex* verts)
{
	unsigned int numVerts = width * height;
	
	std::vector<int> heights(numVerts);

	for (int z = 0; z < height; z++)
	{
		for (int x = 0; x < width; x++)
		{
			int index = z * width + x;

			verts[index] = {};

			verts[index].Position.x = (x - (width / 2.f)) * xScale;
			verts[index].Position.y = (heightmap[index] / 255.f) * yScale;
			verts[index].Position.z = (z - (height / 2.f)) * zScale;

			verts[index].Normal.x = 0.f;
			verts[index].Normal.y = 1.f;
			verts[index].Normal.z = 0.f;

			verts[index].UV.x = x / (float)width;
			verts[index].UV.y = z / (float)height;
		}
	}
}