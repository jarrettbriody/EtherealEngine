#include "DebugLines.h"

vector<DebugLines*> DebugLines::debugLines;
map<string, DebugLines*> DebugLines::debugLinesMap;

DebugLines::DebugLines(string entityName, int colliderID, bool willUpdate)
{
	this->entityName = entityName;
	this->colliderID = colliderID;
	this->willUpdate = willUpdate;
}

DebugLines::~DebugLines()
{
	if (vertices != nullptr) delete[] vertices;
	if (indices != nullptr) delete[] indices;
	if (vertexBuffer != nullptr) vertexBuffer->Release();
	if (indexBuffer != nullptr) indexBuffer->Release();
}

void DebugLines::GenerateCuboidVertexBuffer(XMFLOAT3* verts, int vertCount)
{
	if (vertices != nullptr) delete[] vertices;

	vertices = new DebugLinesVertex[vertCount];
	vertexCount = vertCount;

	for (size_t j = 0; j < vertCount; j++)
	{
		vertices[j].Position = verts[j];
		vertices[j].Color = color;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(DebugLinesVertex) * vertCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;

	//EtherealEngine::GetInstance()->GetDevice()->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);
	Config::Device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);

	
	indexCount = 24;
	indices = new UINT[24];
	UINT i[24] = { 0, 1, 1, 2, 2, 3, 3, 0, 3, 4, 4, 5, 5, 6, 6, 7, 7, 4, 7, 2, 6, 1, 0, 5 };
	memcpy(indices, i, sizeof(UINT) * indexCount);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * indexCount;				// 3 = number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;			// Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;

	Config::Device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer);

	debugLines.push_back(this);
	debugLinesMap.insert({ entityName,this });
}
