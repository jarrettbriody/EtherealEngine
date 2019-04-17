#include "Entity.h"


Entity::Entity(Mesh* entityMesh, Material* mat)
{
	mesh = entityMesh;
	position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W));
	if (mat != nullptr)
		materialMap.insert({ mat->GetName(), mat });
}


Entity::~Entity()
{
}

DirectX::XMFLOAT4X4 Entity::GetWorldMatrix()
{
	return worldMatrix;
}

DirectX::XMFLOAT3 Entity::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Entity::GetScale()
{
	return scale;
}

DirectX::XMFLOAT3 Entity::GetRotation()
{
	return rotation;
}

void Entity::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Entity::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void Entity::SetRotation(float x, float y, float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
}

void Entity::Move(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	CalcWorldMatrix();
}

ID3D11Buffer * Entity::GetMeshVertexBuffer(int i)
{
	if(i == -1)
		return mesh->GetVertexBuffer();
	else
		return mesh->GetChildren()[i]->GetVertexBuffer();
}

ID3D11Buffer * Entity::GetMeshIndexBuffer(int i)
{
	if (i == -1)
		return mesh->GetIndexBuffer();
	else
		return mesh->GetChildren()[i]->GetIndexBuffer();
}

int Entity::GetMeshIndexCount(int i)
{
	if (i == -1)
		return mesh->GetIndexCount();
	else
		return mesh->GetChildren()[i]->GetIndexCount();
}

string Entity::GetMeshMaterialName(int i)
{
	if (i == -1)
		return mesh->GetFirstMaterialName();
	else
		return mesh->GetChildren()[i]->GetFirstMaterialName();
}

void Entity::CalcWorldMatrix()
{
	DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX rot   = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMMATRIX scl   = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX world = scl * rot * trans;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(world));
}

void Entity::PrepareMaterial(string n, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj)
{
	Material* crntMat = materialMap[n];
	SimpleVertexShader* vShader = crntMat->GetVertexShader();
	SimplePixelShader* pShader = crntMat->GetPixelShader();

	// Send data to shader variables
		//  - Do this ONCE PER OBJECT you're drawing
		//  - This is actually a complex process of copying data to a local buffer
		//    and then copying that entire buffer to the GPU.  
		//  - The "SimpleShader" class handles all of that for you.
	vShader->SetMatrix4x4("world", GetWorldMatrix());
	vShader->SetMatrix4x4("view", view);
	vShader->SetMatrix4x4("projection", proj);

	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	vShader->SetShader();
	pShader->SetShader();

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	vShader->CopyAllBufferData();
	pShader->CopyAllBufferData();

	pShader->SetSamplerState("BasicSampler", crntMat->GetSamplerState());
	pShader->SetShaderResourceView("DiffuseTexture", crntMat->GetMaterialData().DiffuseTextureMapSRV);
}

Material * Entity::GetMaterial(string n)
{
	return materialMap[n];
}

bool Entity::MeshHasChildren()
{
	return mesh->HasChildren();
}

int Entity::GetMeshChildCount()
{
	return mesh->GetChildCount();
}

vector<string> Entity::GetMaterialNameList()
{
	return mesh->GetMaterialNameList();
}

void Entity::AddMaterial(Material * mat)
{
	materialMap.insert({ mat->GetName(),mat });
}
