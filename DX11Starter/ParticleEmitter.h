#pragma once
#include "pch.h"
#include "Config.h"
#include "Particles.h"

using namespace DirectX;
using namespace std;

struct ParticleEmitterDescription {
	string emitterName = "DEFAULT";

	string parentName = "";
	XMFLOAT4X4* parentWorld = nullptr;

	XMFLOAT3 emitterPosition = ZERO_VECTOR3; //position of particle emission point
	XMFLOAT3 emissionRotation = ZERO_VECTOR3; //direction of the emission of particles
	XMFLOAT3 emitterScale = ONE_VECTOR3; //scale of the emitter, scales all newly emitted particles
	XMFLOAT3 emitterDirection = ZERO_VECTOR3;
	unsigned int maxParticles = 1000; //set once when emitter is created, unchangeable thereafter
	float emissionStartRadius = 0.001f;
	float emissionEndRadius = 1.0f;
	float emissionRate = 10.0f; //particles per second

	float particleMinLifetime = 10.0f; //minimum lifetime of emitted particles in seconds
	float particleMaxLifetime = 15.0f; //maximum lifetime of emitted particles in seconds
	float particleInitMinScale = 0.25f; //minimum initial scale of the particle
	float particleInitMaxScale = 0.5f; //maximum initial scale of the particle
	float particleInitMinAngularVelocity = 0.0f; //minimum initial angular velocity of the particle
	float particleInitMaxAngularVelocity = 0.0f; //maximum initial angular velocity of the particle
	float particleInitMinSpeed = 0.5f; //minimum initial speed of the particle
	float particleInitMaxSpeed = 0.5f; //maximum initial speed of the particle

	XMFLOAT3 particleAcceleration = ZERO_VECTOR3;

	unsigned int colorCount = 0; //number of possible colors
	ParticleColor* colors = nullptr; //array of ParticleColor objects

	unsigned int textureCount = 0; //number of possible colors
	ParticleTexture* textures = nullptr; //array of ParticleColor objects
};

class ParticleEmitter
{
public:
	static vector<ParticleEmitter*> EmitterVector;
	static map<string, ParticleEmitter*> EmitterMap;
	static map<string, map<string, ParticleEmitter*>> EntityEmitterMap;

	bool isAlive = true;

	virtual ~ParticleEmitter();

	static void KillEmitters(string entityName);

	void CalcWorldMatrix();
	XMFLOAT4X4 GetWorldMatrix();

	void SetName(string name);

	void SetParent(string parentName, XMFLOAT4X4* parentWorld);

	void SetPosition(XMFLOAT3 position);
	void SetRotationDegrees(XMFLOAT3 rotation);
	void SetRotationRadians(XMFLOAT3 rotation);
	void SetRotationQuaternion(XMFLOAT4 quaternion);
	void RotateAroundAxis(XMFLOAT3 axis, float scalar);
	void SetDirectionVector(XMFLOAT3 direction);
	void SetScale(XMFLOAT3 scale);

	void SetEmissionAngleDegrees(float emissionAngleDegrees);
	void SetEmissionAngleRadians(float emissionAngleRadians);

	void SetEmissionRadii(float start, float end);

	//Emission rate in particles per second
	void SetEmissionRate(float emissionRate);
	void SetParticleLifetime(float min, float max);
	void SetParticleInitialScale(float min, float max);
	void SetParticleInitialAngularVelocity(float min, float max);
	void SetParticleInitialSpeed(float min, float max);

	void SetParticleAcceleration(XMFLOAT3 accel);

	//Set colors of particles (number of colors, color array, array of color weights in range [0.0f,1.0f])
	void SetParticleColors(unsigned int colorCount, ParticleColor* colors);

	void SetParticleTextures(unsigned int textureCount, ParticleTexture* textures);

	string GetName();

	virtual void Update(double deltaTime, double totalTime, XMFLOAT4X4 view = XMFLOAT4X4());
	virtual void Draw(XMFLOAT4X4 view, XMFLOAT4X4 proj) = 0;

protected:
	ParticleEmitter();
	ParticleEmitter(ParticleEmitterDescription d);

	ID3D11ShaderResourceView* texturesSRV = nullptr;

	string name;
	string parentName;
	XMFLOAT4X4* parentWorld;

	float lifetime = 0.0f;
	float maxLifetime = 0.0f;

	float emitTimeCounter = 0.0f;

	XMFLOAT4X4 worldMatrix = MATRIX_IDENTITY;
	XMFLOAT3 position = ZERO_VECTOR3;
	XMFLOAT3 rotationDegrees = ZERO_VECTOR3;
	XMFLOAT3 rotationRadians = ZERO_VECTOR3;
	XMFLOAT4 rotationQuaternion;
	XMFLOAT3 direction = Y_AXIS;
	XMFLOAT3 scale = ONE_VECTOR3;

	float emissionStartRadius = 0.0f;
	float emissionEndRadius = 0.0f;
	float emissionAngleDegrees = 0.0f;
	float emissionAngleRadians = 0.0f;
	unsigned int maxParticles = 0;
	float emissionRate = 0.0f;
	float particleMinLifetime = 0.0f;
	float particleMaxLifetime = 0.0f;
	float particleInitMinScale = 0.0f; //minimum initial scale of the particle
	float particleInitMaxScale = 0.0f; //maximum initial scale of the particle
	float particleInitMinAngularVelocity = 0.0f; //minimum initial angular velocity of the particle
	float particleInitMaxAngularVelocity = 0.0f; //maximum initial angular velocity of the particle
	float particleInitMinSpeed = 0.0f; //minimum initial speed of the particle
	float particleInitMaxSpeed = 0.0f; //maximum initial speed of the particle

	XMFLOAT3 particleAcceleration = ZERO_VECTOR3;

	unsigned int colorCount = 0;
	ParticleColor colors[MAX_PARTICLE_COLORS];

	unsigned int textureCount = 0;
	ParticleTexture textures[MAX_PARTICLE_TEXTURES];
	ParticleTextureToGPU texturesToGPU[MAX_PARTICLE_TEXTURES];

	void CalcEulerAngles();
	void SetMaxParticles(unsigned int maxParticles);
};
