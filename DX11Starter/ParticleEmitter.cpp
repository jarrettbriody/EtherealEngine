#include "pch.h"
#include "ParticleEmitter.h"

#pragma region ParticleEmitter

map<string, map<string, ParticleEmitter*>> ParticleEmitter::EntityEmitterMap;
vector<ParticleEmitter*> ParticleEmitter::EmitterVector;
map<string, ParticleEmitter*> ParticleEmitter::EmitterMap;

ParticleEmitter::ParticleEmitter()
{
	ParticleEmitterDescription d;

	SetName(d.emitterName);
	SetParent(d.parentName, d.parentWorld);
	SetPosition(d.emitterPosition);
	SetRotationRadians(d.emissionRotation);
	SetScale(d.emitterScale);
	SetEmissionRadii(d.emissionStartRadius, d.emissionEndRadius);
	SetMaxParticles(d.maxParticles);
	SetEmissionRate(d.emissionRate);
	SetParticleLifetime(d.particleMinLifetime, d.particleMaxLifetime);
	SetParticleInitialScale(d.particleInitMinScale, d.particleInitMaxScale);
	SetParticleInitialAngularVelocity(d.particleInitMinAngularVelocity, d.particleInitMaxAngularVelocity);
	SetParticleInitialSpeed(d.particleInitMinSpeed, d.particleInitMaxSpeed);
	SetParticleAcceleration(d.particleAcceleration);
	ParticleColor color[1] = { XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), 1.0f };
	SetParticleColors(1, color);

	CalcWorldMatrix();

	EmitterVector.push_back(this);
	EmitterMap.insert({ this->name, this });
}

ParticleEmitter::ParticleEmitter(ParticleEmitterDescription d)
{
	SetName(d.emitterName);
	SetParent(d.parentName, d.parentWorld);
	SetPosition(d.emitterPosition);
	SetRotationRadians(d.emissionRotation);
	if (d.emitterDirection.x != 0.0f || d.emitterDirection.y != 0.0f || d.emitterDirection.z != 0.0f) {
		SetDirectionVector(d.emitterDirection);
	}
	SetScale(d.emitterScale);
	SetEmissionRadii(d.emissionStartRadius, d.emissionEndRadius);
	SetMaxParticles(d.maxParticles);
	SetEmissionRate(d.emissionRate);
	SetParticleLifetime(d.particleMinLifetime, d.particleMaxLifetime);
	SetParticleInitialScale(d.particleInitMinScale, d.particleInitMaxScale);
	SetParticleInitialAngularVelocity(d.particleInitMinAngularVelocity, d.particleInitMaxAngularVelocity);
	SetParticleInitialSpeed(d.particleInitMinSpeed, d.particleInitMaxSpeed);
	SetParticleAcceleration(d.particleAcceleration);
	SetParticleColors(d.colorCount, d.colors);
	SetParticleTextures(d.textureCount, d.textures);

	CalcWorldMatrix();

	EmitterVector.push_back(this);
	EmitterMap.insert({ this->name, this });

	if (this->parentName != "" && this->parentWorld != nullptr) {
		if(!EntityEmitterMap.count(this->parentName)) EntityEmitterMap.insert({ this->parentName, map<string, ParticleEmitter*>() });
		EntityEmitterMap[this->parentName].insert({ this->name, this });
	}
}

ParticleEmitter::~ParticleEmitter()
{
	if (texturesSRV != nullptr) texturesSRV->Release();
}

void ParticleEmitter::KillEmitters(string entityName)
{
	if (EntityEmitterMap.count(entityName)) {
		for (auto iter = EntityEmitterMap[entityName].begin(); iter != EntityEmitterMap[entityName].end(); ++iter)
		{
			iter->second->isAlive = false;
		}
		EntityEmitterMap.erase(entityName);
	}
}

void ParticleEmitter::CalcWorldMatrix()
{
	DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	XMVECTOR quat = XMLoadFloat4(&rotationQuaternion);
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationQuaternion(quat);
	DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX world = scl * rot * trans;
	if (parentWorld != nullptr) {
		XMFLOAT4X4 pWorld = *parentWorld;
		DirectX::XMMATRIX parentWorld = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&pWorld));
		world = DirectX::XMMatrixMultiply(world, parentWorld);
	}
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixTranspose(world));
}

XMFLOAT4X4 ParticleEmitter::GetWorldMatrix()
{
	return worldMatrix;
}

void ParticleEmitter::SetName(string name)
{
	this->name = name;
	//check if emitter name already exists, if it does then add (1), (2), etc
	int sameNameCnt = 1;
	while (EmitterMap.count(this->name)) {
		this->name = name + " (" + to_string(sameNameCnt) + ")";
		sameNameCnt++;
	}
}

void ParticleEmitter::SetParent(string parentName, XMFLOAT4X4* parentWorld)
{
	this->parentName = parentName;
	this->parentWorld = parentWorld;
}

void ParticleEmitter::SetPosition(XMFLOAT3 position)
{
	this->position = position;
}

void ParticleEmitter::SetRotationDegrees(XMFLOAT3 rotation)
{
	rotationDegrees = rotation;

	rotationRadians = XMFLOAT3(DirectX::XMConvertToRadians(rotation.x), DirectX::XMConvertToRadians(rotation.y), DirectX::XMConvertToRadians(rotation.z));

	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(rotationRadians.x, rotationRadians.y, rotationRadians.z);
	XMStoreFloat4(&rotationQuaternion, quat);

	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR dir = XMLoadFloat3(&yAxis);
	dir = XMVector3Transform(dir, XMMatrixRotationQuaternion(quat));
	XMStoreFloat3(&direction, dir);
}

void ParticleEmitter::SetRotationRadians(XMFLOAT3 rotation)
{
	rotationRadians = rotation;

	rotationDegrees = XMFLOAT3(DirectX::XMConvertToDegrees(rotation.x), DirectX::XMConvertToDegrees(rotation.y), DirectX::XMConvertToDegrees(rotation.z));

	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(rotationRadians.x, rotationRadians.y, rotationRadians.z);
	XMStoreFloat4(&rotationQuaternion, quat);

	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR dir = XMLoadFloat3(&yAxis);
	dir = XMVector3Transform(dir, XMMatrixRotationQuaternion(quat));
	XMStoreFloat3(&direction, dir);
}

void ParticleEmitter::SetRotationQuaternion(XMFLOAT4 quaternion)
{
	rotationQuaternion = quaternion;
	
	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR dir = XMLoadFloat3(&yAxis);
	dir = XMVector3Transform(dir, XMMatrixRotationQuaternion(XMLoadFloat4(&quaternion)));
	XMStoreFloat3(&direction, dir);
}

void ParticleEmitter::RotateAroundAxis(XMFLOAT3 axis, float scalar)
{
	XMVECTOR a = XMLoadFloat3(&axis);
	XMVECTOR quat = XMQuaternionRotationAxis(a, scalar);
	XMVECTOR existingQuat = XMLoadFloat4(&rotationQuaternion);
	XMVECTOR result = XMQuaternionMultiply(existingQuat, quat);
	XMStoreFloat4(&rotationQuaternion, result);

	CalcEulerAngles();

	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR dir = XMLoadFloat3(&yAxis);
	dir = XMVector3Transform(dir, XMMatrixRotationQuaternion(result));
	XMStoreFloat3(&direction, dir);
}

void ParticleEmitter::SetDirectionVector(XMFLOAT3 direction)
{
	this->direction = direction;

	XMVECTOR dir = XMLoadFloat3(&direction);
	dir = XMVector3Normalize(dir);
	XMFLOAT3 y = Y_AXIS;
	XMVECTOR up = XMLoadFloat3(&y);
	XMVECTOR right = XMVector3Cross(up, dir);
	right = XMVector3Normalize(right);
	up = XMVector3Cross(dir, right);
	up = XMVector3Normalize(up);
	XMFLOAT4 botRow(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR trans = XMLoadFloat4(&botRow);
	XMMATRIX rotation = XMMATRIX(right, up, dir, trans);

	XMVECTOR quat = XMQuaternionRotationMatrix(rotation);

	XMStoreFloat4(&rotationQuaternion, quat);

	CalcEulerAngles();
}

void ParticleEmitter::SetScale(XMFLOAT3 scale)
{
	this->scale = scale;
}

void ParticleEmitter::SetEmissionAngleDegrees(float emissionAngleDegrees)
{
	this->emissionAngleDegrees = emissionAngleDegrees;
	this->emissionAngleRadians = XMConvertToRadians(emissionAngleDegrees);
}

void ParticleEmitter::SetEmissionAngleRadians(float emissionAngleRadians)
{
	this->emissionAngleRadians = emissionAngleRadians;
	this->emissionAngleDegrees = XMConvertToDegrees(emissionAngleRadians);
}

void ParticleEmitter::SetEmissionRadii(float start, float end)
{
	emissionStartRadius = start;
	emissionEndRadius = end;
}

void ParticleEmitter::SetMaxParticles(unsigned int maxParticles)
{
	this->maxParticles = maxParticles;
}

void ParticleEmitter::SetEmissionRate(float emissionRate)
{
	this->emissionRate = 1.0f / emissionRate;
}

void ParticleEmitter::SetParticleLifetime(float min, float max)
{
	this->particleMinLifetime = min;
	this->particleMaxLifetime = max;
}

void ParticleEmitter::SetParticleInitialScale(float min, float max)
{
	this->particleInitMinScale = min;
	this->particleInitMaxScale = max;
}

void ParticleEmitter::SetParticleInitialAngularVelocity(float min, float max)
{
	this->particleInitMinAngularVelocity = min;
	this->particleInitMaxAngularVelocity = max;
}

void ParticleEmitter::SetParticleInitialSpeed(float min, float max)
{
	this->particleInitMinSpeed = min;
	this->particleInitMaxSpeed = max;
}

void ParticleEmitter::SetParticleAcceleration(XMFLOAT3 accel)
{
	particleAcceleration = accel;
}

void ParticleEmitter::SetParticleColors(unsigned int colorCount, ParticleColor* colors)
{
	if (colorCount > MAX_PARTICLE_COLORS) return;
	this->colorCount = colorCount;
	if (colors == nullptr) return;
	float total = 0.0f;

	for (size_t i = 0; i < colorCount; i++)
	{
		total += colors[i].weight;
	}
	for (size_t i = 0; i < textureCount; i++)
	{
		total += textures[i].weight;
	}

	for (size_t i = 0; i < colorCount; i++)
	{
		colors[i].weight = colors[i].weight / total;
	}
	for (size_t i = 0; i < textureCount; i++)
	{
		textures[i].weight = textures[i].weight / total;
	}

	for (int i = 1; i < (int)colorCount; i++)
	{
		colors[i].weight += colors[i - 1].weight;
	}
	if (colorCount > 0 && textureCount > 0) textures[0].weight += colors[colorCount - 1].weight;
	for (int i = 1; i < (int)textureCount; i++)
	{
		textures[i].weight += textures[i - 1].weight;
	}

	memcpy(this->colors, colors, sizeof(ParticleColor) * (size_t)colorCount);
}

void ParticleEmitter::SetParticleTextures(unsigned int textureCount, ParticleTexture* textures)
{
	if (textureCount > MAX_PARTICLE_TEXTURES || textureCount == 0) return;
	this->textureCount = textureCount;
	if (textures == nullptr) return;
	float total = 0.0f;

	for (size_t i = 0; i < textureCount; i++)
	{
		total += textures[i].weight;
	}
	for (size_t i = 0; i < colorCount; i++)
	{
		total += colors[i].weight;
	}

	for (size_t i = 0; i < textureCount; i++)
	{
		textures[i].weight = textures[i].weight / total;
	}
	for (size_t i = 0; i < colorCount; i++)
	{
		colors[i].weight = colors[i].weight / total;
	}

	for (int i = 1; i < (int)colorCount; i++)
	{
		colors[i].weight += colors[i - 1].weight;
	}
	if (colorCount > 0 && textureCount > 0) textures[0].weight += colors[colorCount - 1].weight;
	for (int i = 1; i < (int)textureCount; i++)
	{
		textures[i].weight += textures[i - 1].weight;
	}

	memcpy(this->textures, textures, sizeof(ParticleTexture) * (size_t)textureCount);

	for (int i = 0; i < textureCount; i++)
	{
		texturesToGPU[i] = { i, this->textures[i].weight };
	}

	HRESULT hr;

	D3D11_TEXTURE2D_DESC firstTexDesc;
	this->textures[0].texture->GetDesc(&firstTexDesc);

	// Set up the texture itself
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Height = firstTexDesc.Height;
	texDesc.Width = firstTexDesc.Width;
	texDesc.MipLevels = firstTexDesc.MipLevels;
	texDesc.ArraySize = textureCount;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// Actually create the texture
	ID3D11Texture2D* textureArray;
	hr = Config::Device->CreateTexture2D(&texDesc, 0, &textureArray);

	for (size_t i = 0; i < textureCount; i++)
	{
		for (size_t j = 0; j < firstTexDesc.MipLevels; ++j)
		{
			D3D11_MAPPED_SUBRESOURCE MappedResource;
			hr = Config::Context->Map(this->textures[i].texture, j, D3D11_MAP_READ, 0, &MappedResource);
			Config::Context->UpdateSubresource(textureArray, D3D11CalcSubresource(j, i, firstTexDesc.MipLevels), 0, MappedResource.pData, MappedResource.RowPitch, 0);
			Config::Context->Unmap(this->textures[i].texture, j);
		}
	}

	if (texturesSRV != nullptr) texturesSRV->Release();
	// Create the shader resource view for this texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Texture2DArray.MipLevels = firstTexDesc.MipLevels;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.ArraySize = textureCount;
	hr = Config::Device->CreateShaderResourceView(textureArray, &srvDesc, &texturesSRV);

	textureArray->Release();
}

string ParticleEmitter::GetName()
{
	return name;
}

void ParticleEmitter::Update(double deltaTime, double totalTime, XMFLOAT4X4 view)
{
	lifetime += deltaTime;
	emitTimeCounter += deltaTime;
	isAlive = (maxLifetime == 0.0f || lifetime < maxLifetime);
	CalcWorldMatrix();
}

void ParticleEmitter::CalcEulerAngles()
{
	XMVECTOR q = XMLoadFloat4(&rotationQuaternion);

	XMFLOAT3 x = X_AXIS;
	XMFLOAT3 y = X_AXIS;
	XMFLOAT3 z = X_AXIS;

	XMVECTOR xs = XMLoadFloat3(&x);
	XMVECTOR ys = XMLoadFloat3(&y);
	XMVECTOR zs = XMLoadFloat3(&z);

	XMQuaternionToAxisAngle(&xs, &rotationRadians.x, q);
	XMQuaternionToAxisAngle(&ys, &rotationRadians.y, q);
	XMQuaternionToAxisAngle(&zs, &rotationRadians.z, q);

	rotationDegrees = XMFLOAT3(DirectX::XMConvertToDegrees(rotationRadians.x), DirectX::XMConvertToDegrees(rotationRadians.y), DirectX::XMConvertToDegrees(rotationRadians.z));
}