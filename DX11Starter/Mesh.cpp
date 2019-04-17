#include "Mesh.h"
#include <iostream>

using namespace DirectX;

vector<string> Mesh::mtlPaths;

Mesh::Mesh()
{
}

Mesh::Mesh(Vertex * vertexObjects, int vertexCount, unsigned int * indices, int indexCnt, ID3D11Device * device, string meshN, string matName)
{
	meshName = meshN;
	materialNameList.push_back(matName);
	CreateBuffers(vertexObjects, vertexCount, indices, indexCnt, device);
}

Mesh::Mesh(string meshN, char * objFile, ID3D11Device* device)
{
	meshName = meshN;
	// File input object
	std::ifstream obj(objFile);

	// Check for successful open
	if (!obj.is_open())
		return;

	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts
	unsigned int vertCounter = 0;        // Count of vertices/indices
	char chars[200];                     // String for line reading
	string line;
	bool isGroup = false;
	string groupName = "";
	string matName = "";
	regex useMatRgx("^(usemtl )");
	regex mtllibRgx("^(mtllib )");
	smatch match;

	// Still have data left?
	while (obj.good())
	{
		// Get the line (100 characters should be more than enough)
		obj.getline(chars, 200);

		line = string(chars);

		//new group, push current group to children
		if (isGroup && chars[0] == 'v') {
			materialNameList.push_back(matName);
			children.push_back(new Mesh(&verts[0], vertCounter, &indices[0], vertCounter, device, groupName, matName));
			childCount++;
			//reset everything
			isGroup = false;
			groupName = "";
			matName = "";
			verts.clear();
			indices.clear();
			vertCounter = 0;
		}

		// Check the type of line
		if (chars[0] == 'v' && chars[1] == 'n')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(
				chars,
				"vn %f %f %f",
				&norm.x, &norm.y, &norm.z);

			// Add to the list of normals
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't')
		{
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(
				chars,
				"vt %f %f",
				&uv.x, &uv.y);

			// Add to the list of uv's
			uvs.push_back(uv);
		}
		else if (chars[0] == 'v')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(
				chars,
				"v %f %f %f",
				&pos.x, &pos.y, &pos.z);

			// Add to the positions
			positions.push_back(pos);
		}
		else if (chars[0] == 'f')
		{
			// Read the face indices into an array
			unsigned int i[12];
			int facesRead = sscanf_s(
				chars,
				"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8],
				&i[9], &i[10], &i[11]);

			// - Create the verts by looking up
			//    corresponding data from vectors
			// - OBJ File indices are 1-based, so
			//    they need to be adusted
			Vertex v1;
			v1.Position = positions[i[0] - 1];
			v1.UV = uvs[i[1] - 1];
			v1.Normal = normals[i[2] - 1];

			Vertex v2;
			v2.Position = positions[i[3] - 1];
			v2.UV = uvs[i[4] - 1];
			v2.Normal = normals[i[5] - 1];

			Vertex v3;
			v3.Position = positions[i[6] - 1];
			v3.UV = uvs[i[7] - 1];
			v3.Normal = normals[i[8] - 1];

			// The model is most likely in a right-handed space,
			// especially if it came from Maya.  We want to convert
			// to a left-handed space for DirectX.  This means we 
			// need to:
			//  - Invert the Z position
			//  - Invert the normal's Z
			//  - Flip the winding order
			// We also need to flip the UV coordinate since DirectX
			// defines (0,0) as the top left of the texture, and many
			// 3D modeling packages use the bottom left as (0,0)

			// Flip the UV's since they're probably "upside down"
			v1.UV.y = 1.0f - v1.UV.y;
			v2.UV.y = 1.0f - v2.UV.y;
			v3.UV.y = 1.0f - v3.UV.y;

			// Flip Z (LH vs. RH)
			v1.Position.z *= -1.0f;
			v2.Position.z *= -1.0f;
			v3.Position.z *= -1.0f;

			// Flip normal Z
			v1.Normal.z *= -1.0f;
			v2.Normal.z *= -1.0f;
			v3.Normal.z *= -1.0f;

			// Add the verts to the vector (flipping the winding order)
			verts.push_back(v1);
			verts.push_back(v3);
			verts.push_back(v2);

			// Add three more indices
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;

			// Was there a 4th face?
			if (facesRead == 12)
			{
				// Make the last vertex
				Vertex v4;
				v4.Position = positions[i[9] - 1];
				v4.UV = uvs[i[10] - 1];
				v4.Normal = normals[i[11] - 1];

				// Flip the UV, Z pos and normal
				v4.UV.y = 1.0f - v4.UV.y;
				v4.Position.z *= -1.0f;
				v4.Normal.z *= -1.0f;

				// Add a whole triangle (flipping the winding order)
				verts.push_back(v1);
				verts.push_back(v4);
				verts.push_back(v3);

				// Add three more indices
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
			}
		}
		else if (chars[0] == 'g') {
			isGroup = true;
			groupName = line.substr(2);
		}
		else if (regex_search(line, match, useMatRgx)) {
			matName = regex_replace(line, useMatRgx, "");
			materialNameList.push_back(matName);
		}
		else if (regex_search(line, match, mtllibRgx)) {
			mtlPath = regex_replace(line, mtllibRgx, "");
			mtlPaths.push_back(mtlPath);
		}
	}

	obj.close();

	if (isGroup && groupName != "" && matName != "" && childCount > 0) {
		materialNameList.push_back(matName);
		children.push_back(new Mesh(&verts[0], vertCounter, &indices[0], vertCounter, device, groupName, matName));
		childCount++;
	}
	else if(childCount == 0)
		CreateBuffers(&verts[0], vertCounter, &indices[0], vertCounter, device);
}


Mesh::~Mesh()
{
	if (childCount == 0) {
		vertexBuffer->Release();
		indexBuffer->Release();
	}
	for (size_t i = 0; i < childCount; i++)
	{
		delete children[i];
	}
}

ID3D11Buffer * Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

ID3D11Buffer * Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return indexCount;
}

void Mesh::CreateBuffers(Vertex* vertexObjects, int vertexCount, unsigned int* indices, int indexCnt, ID3D11Device* device)
{
	indexCount = indexCnt;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vertexCount;       // 3 = number of vertices in the buffer
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;			// Tells DirectX this is a vertex buffer
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertexObjects;

	device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * indexCnt;				// 3 = number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;			// Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;

	device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer);
}

vector<string> Mesh::GetMaterialNameList()
{
	return materialNameList;
}

string Mesh::GetFirstMaterialName()
{
	if (materialNameList.size() > 0)
		return materialNameList[0];
	else return "There are no materials assigned to this mesh.";
}

bool Mesh::HasChildren()
{
	return childCount != 0;
}

vector<Mesh*> Mesh::GetChildren()
{
	return children;
}

int Mesh::GetChildCount()
{
	return childCount;
}

vector<string>* Mesh::GetMtlPaths()
{
	return &mtlPaths;
}
