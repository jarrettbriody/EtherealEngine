#include "pch.h"
#include "Mesh.h"

using namespace DirectX;

vector<EEString<64>> Mesh::mtlPaths;

Mesh::Mesh()
{
}

Mesh::Mesh(Vertex * vertexObjects, int vertexCount, unsigned int * indices, int indexCnt, string meshN, string matName)
{
	vertices = new vector<DirectX::XMFLOAT3>();
	childrenVec = new vector<Mesh*>();
	materialNameList = new vector<string>();
	for (size_t i = 0; i < vertexCount; i++)
	{
		vertices->push_back(vertexObjects[i].Position);
	}
	meshName = meshN;
	materialNameList->push_back(matName);
	CreateBuffers(vertexObjects, vertexCount, indices, indexCnt);
	childCount = 0;
}

Mesh::Mesh(string meshN, char * objFile, bool* success)
{
	vertices = new vector<DirectX::XMFLOAT3>();
	childrenVec = new vector<Mesh*>();
	materialNameList = new vector<string>();

	meshName = meshN;
	// File input object
	std::ifstream obj(objFile);

	// Check for successful open
	if (!obj.is_open()) {
		if(success != nullptr)
			*success = false;
		return;
	}

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

		// Check the type of line
		if (chars[0] == 'g') {
			if (!isGroup && line != "g default") {
				isGroup = true;
				groupName = line.substr(2);
			}
			else if (isGroup && line != "g default") {
				materialNameList->push_back(matName);
				groupName = line.substr(2);
				Mesh* newChild = new Mesh(&verts[0], vertCounter, &indices[0], vertCounter, groupName, matName);
				childrenVec->push_back(newChild);
				childCount++;
				//reset everything
				matName = "";
				verts.clear();
				indices.clear();
				vertCounter = 0;
				isGroup = true;
				groupName = line.substr(2);
			}
		}
		else if (chars[0] == 'v' && chars[1] == 'n')
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
		else if (regex_search(line, match, useMatRgx)) {
			matName = regex_replace(line, useMatRgx, "");
			materialNameList->push_back(matName);
		}
		else if (regex_search(line, match, mtllibRgx)) {
			mtlPath = regex_replace(line, mtllibRgx, "");
			mtlPaths.push_back(mtlPath);
		}
	}

	obj.close();

	if (isGroup && groupName != "" && matName != "" && childCount > 0 && groupName != "default") {
		materialNameList->push_back(matName);
		Mesh* newChild = new Mesh(&verts[0], vertCounter, &indices[0], vertCounter, groupName, matName);
		childrenVec->push_back(newChild);
		childCount++;
	}
	else if (childCount == 0) {
		for (size_t i = 0; i < verts.size(); i++)
		{
			vertices->push_back(verts[i].Position);
		}
		CreateBuffers(&verts[0], vertCounter, &indices[0], vertCounter);
	}
	if (success != nullptr)
		*success = true;
}


Mesh::~Mesh()
{
	if (children != nullptr) {
		delete[] children;
		children = nullptr;
	}
		
	if (vertices != nullptr) {
		delete vertices;
		vertices = nullptr;
	}
		
	if (childrenVec != nullptr) {
		delete childrenVec;
		childrenVec = nullptr;
	}
		
	if (materialNameList != nullptr) {
		delete materialNameList;
		materialNameList = nullptr;
	}
		
}

void Mesh::operator=(const Mesh& m)
{
	vertices = new vector<DirectX::XMFLOAT3>();
	childrenVec = new vector<Mesh*>();
	materialNameList = new vector<string>();

	*vertices = vector<DirectX::XMFLOAT3>(*m.vertices);
	vertexBuffer = m.vertexBuffer;
	indexBuffer = m.indexBuffer;
	indexCount = m.indexCount;
	*childrenVec = vector<Mesh*>(*m.childrenVec);
	mtlPath = m.mtlPath;
	*materialNameList = vector<string>(*m.materialNameList);
	meshName = m.meshName;
	childCount = m.childCount;
	children = nullptr;

	if (m.children != nullptr && childCount > 0) {
		children = new Mesh*[childCount];
		for (size_t i = 0; i < childCount; i++)
		{
			children[i] = m.children[i];
		}
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

void Mesh::CreateBuffers(Vertex* vertexObjects, int vertexCount, unsigned int* indices, int indexCnt)
{
	// Calculate the tangents before copying to buffer
	CalculateTangents(vertexObjects, vertexCount, indices, indexCnt);

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

	Config::Device->CreateBuffer(&vbd, &initialVertexData, &vertexBuffer);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * indexCnt;				// 3 = number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;			// Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;

	Config::Device->CreateBuffer(&ibd, &initialIndexData, &indexBuffer);
}

// Calculates the tangents of the vertices in a mesh
// Code adapted from: http://www.terathon.com/code/tangent.html
void Mesh::CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices)
{
	// Reset tangents
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].Tangent = XMFLOAT3(0, 0, 0);
	}

	// Calculate tangents one whole triangle at a time
	for (int i = 0; i < numVerts;)
	{
		// Grab indices and vertices of first triangle
		unsigned int i1 = indices[i++];
		unsigned int i2 = indices[i++];
		unsigned int i3 = indices[i++];
		Vertex* v1 = &verts[i1];
		Vertex* v2 = &verts[i2];
		Vertex* v3 = &verts[i3];

		// Calculate vectors relative to triangle positions
		float x1 = v2->Position.x - v1->Position.x;
		float y1 = v2->Position.y - v1->Position.y;
		float z1 = v2->Position.z - v1->Position.z;

		float x2 = v3->Position.x - v1->Position.x;
		float y2 = v3->Position.y - v1->Position.y;
		float z2 = v3->Position.z - v1->Position.z;

		// Do the same for vectors relative to triangle uv's
		float s1 = v2->UV.x - v1->UV.x;
		float t1 = v2->UV.y - v1->UV.y;

		float s2 = v3->UV.x - v1->UV.x;
		float t2 = v3->UV.y - v1->UV.y;

		// Create vectors for tangent calculation
		float r = 1.0f / (s1 * t2 - s2 * t1);

		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		// Adjust tangents of each vert of the triangle
		v1->Tangent.x += tx;
		v1->Tangent.y += ty;
		v1->Tangent.z += tz;

		v2->Tangent.x += tx;
		v2->Tangent.y += ty;
		v2->Tangent.z += tz;

		v3->Tangent.x += tx;
		v3->Tangent.y += ty;
		v3->Tangent.z += tz;
	}

	// Ensure all of the tangents are orthogonal to the normals
	for (int i = 0; i < numVerts; i++)
	{
		// Grab the two vectors
		XMVECTOR normal = XMLoadFloat3(&verts[i].Normal);
		XMVECTOR tangent = XMLoadFloat3(&verts[i].Tangent);

		// Use Gram-Schmidt orthogonalize
		tangent = XMVector3Normalize(
			XMVectorSubtract(tangent, XMVectorMultiply(normal, XMVector3Dot(normal, tangent))));

		// Store the tangent
		XMStoreFloat3(&verts[i].Tangent, tangent);
	}
}

vector<string> Mesh::GetMaterialNameList()
{
	return *materialNameList;
}

string Mesh::GetMaterialName(unsigned int index)
{
	if (materialNameList->size() > 0)
		return (*materialNameList)[index];
	else return "There are no materials assigned to this mesh.";
}

unsigned int Mesh::AddMaterialName(string nm)
{
	for (size_t i = 0; i < materialNameList->size(); i++)
	{
		if ((*materialNameList)[i] == nm) return i;
	}
	materialNameList->push_back(nm);
	return materialNameList->size() - 1;
}

bool Mesh::HasChildren()
{
	return childCount != 0;
}

Mesh** Mesh::GetChildren()
{
	return children;
}

int Mesh::GetChildCount()
{
	return childCount;
}

string Mesh::GetMTLPath()
{
	return mtlPath.STDStr();
}

vector<EEString<64>> Mesh::GetMTLPaths()
{
	return mtlPaths;
}

void Mesh::SetVertices(vector<DirectX::XMFLOAT3> verts)
{
	*vertices = verts;
}

vector<DirectX::XMFLOAT3> Mesh::GetVertices()
{
	return *vertices;
}

void Mesh::FreeMemory()
{
	if (children != nullptr) {
		for (size_t i = 0; i < childCount; i++)
		{
			(children[i])->FreeMemory();
		}
		delete[] children;
	}

	if (vertexBuffer)
		vertexBuffer->Release();
	if (indexBuffer)
		indexBuffer->Release();

	delete vertices;
	delete childrenVec;
	delete materialNameList;
}

void Mesh::ReleaseBuffers()
{
	if (vertexBuffer)
		vertexBuffer->Release();
	if (indexBuffer)
		indexBuffer->Release();
}

void Mesh::AllocateChildren()
{
	if (childCount > 0) {
		MemoryAllocator* mem = MemoryAllocator::GetInstance();
		children = new Mesh * [childCount];
		bool success = false;
		for (size_t i = 0; i < childCount; i++)
		{
			children[i] = (Mesh*)mem->AllocateToPool((unsigned int)MEMORY_POOL::MESH_POOL, sizeof(Mesh), success);
			if (success) {
				*(children[i]) = *(*childrenVec)[i];
				delete (*childrenVec)[i];
			}
		}
		//delete childrenVec;
		//childrenVec = nullptr;
	}
}

string Mesh::GetName()
{
	return meshName.STDStr();
}
