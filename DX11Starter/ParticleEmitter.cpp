#include "pch.h"
#include "ParticleEmitter.h"

#pragma region ParticleEmitter

ParticleEmitter::ParticleEmitter()
{
	ParticleEmitterDescription d;

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
}

ParticleEmitter::ParticleEmitter(ParticleEmitterDescription d)
{
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

	CalcWorldMatrix();
}

ParticleEmitter::~ParticleEmitter()
{
	
}

void ParticleEmitter::CalcWorldMatrix()
{
	DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	XMVECTOR quat = XMLoadFloat4(&rotationQuaternion);
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationQuaternion(quat);
	DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX world = scl * rot * trans;
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixTranspose(world));
}

void ParticleEmitter::CalcWorldMatrix(XMFLOAT4X4 parentWorld)
{
	DirectX::XMMATRIX trans = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	XMVECTOR quat = XMLoadFloat4(&rotationQuaternion);
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationQuaternion(quat);
	DirectX::XMMATRIX scl = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX world = scl * rot * trans;
	DirectX::XMMATRIX parentWorldMat = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&parentWorld));
	world = DirectX::XMMatrixMultiply(world, parentWorldMat);
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixTranspose(world));
}

XMFLOAT4X4 ParticleEmitter::GetWorldMatrix()
{
	return worldMatrix;
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
	for (size_t i = 0; i < colorCount; i++)
	{
		colors[i].weight = colors[i].weight / total;
	}
	for (int i = (int)colorCount - 2; i >= 0; i--)
	{
		colors[i].weight += colors[i + 1].weight;
	}
	memcpy(this->colors, colors, sizeof(ParticleColor) * (size_t)colorCount);
}

void ParticleEmitter::Update(double deltaTime, double totalTime, XMFLOAT4X4 view)
{
	lifetime += deltaTime;
	emitTimeCounter += deltaTime;
	isAlive = (maxLifetime == 0.0f || lifetime < maxLifetime);
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