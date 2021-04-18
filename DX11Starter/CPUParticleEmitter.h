#pragma once
#include "pch.h"
#include "ParticleEmitter.h"
#include "MemoryAllocator.h"
#include "PhysicsWrapper.h"

struct DefaultCPUParticleShaders {
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
};

struct ParticlePhysicsWrapper {
	unsigned int particleIndex;
	void* particle;
	void* emitter;
};

struct ParticlePhysics {
	PhysicsWrapper wrapper;
	ParticlePhysicsWrapper particleWrapper;
	btGhostObject* ghostObject = nullptr;
	btGhostPairCallback* ghostCallback = nullptr;
	btCollisionShape* collShape = nullptr;
	bool enabled = false;
};

struct ParticleCollision {
	unsigned int particleIndex;
	void* particle;
	void* emitter;
	void* hitEntity;
};

class CPUParticleEmitter : ParticleEmitter
{
public:
	static void SetDefaultShaders(DefaultCPUParticleShaders s);

	CPUParticleEmitter();
	CPUParticleEmitter(ParticleEmitterDescription d, bool blendingEnabled = true);
	~CPUParticleEmitter();

	void SetBlendingEnabled(bool toggle);
	void SetCollisionsEnabled(void (*collisionCallback)(void* ptr));
	void KillParticle(unsigned int index);

	void Update(double deltaTime, double totalTime, XMFLOAT4X4 view = XMFLOAT4X4());
	void Draw(XMFLOAT4X4 view, XMFLOAT4X4 proj);

private:
	static DefaultCPUParticleShaders defaultShaders;

	void (*collisionCallback)(void* ptr) = nullptr;
	bool collisionsEnabled = false;

	bool blendingEnabled = true;

	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	ID3D11BlendState* additiveBlend;
	ID3D11DepthStencilState* depthWriteOff;

	Particle* particlePool;
	ParticlePhysics* particlePhysicsPool;
	unsigned int* deadList;
	unsigned int deadListCount;
	ParticleVertex* particleVertices;
	unsigned int* indices;
	unsigned int particleVertCount;
	unsigned int drawCount;

	btGhostPairCallback* ghostCallback = nullptr;

	void InitBuffers();

	void CalcVertex(Particle p, XMFLOAT4X4 view);
};