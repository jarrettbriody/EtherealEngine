#pragma once
#include "pch.h"
#include "ParticleEmitter.h"
#include "MemoryAllocator.h"
#include "PhysicsWrapper.h"

using namespace DirectX;

struct DefaultCPUParticleShaders {
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
};

struct ParticlePhysicsWrapper {
	unsigned int particleIndex = 0;
	void* particle = nullptr;
	void* emitter = nullptr;
};

struct ParticlePhysics {
	PhysicsWrapper wrapper = {};
	ParticlePhysicsWrapper particleWrapper = {};
	btGhostObject* ghostObject = nullptr;
	//btGhostPairCallback* ghostCallback = nullptr;
	btCollisionShape* collShape = nullptr;
	bool enabled = false;
};

struct ParticleCollision {
	unsigned int particleIndex = 0;
	void* particle = nullptr;
	void* emitter = nullptr;
	void* hitEntity = nullptr;
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

	void Destroy();

	void Update(double deltaTime, double totalTime, XMFLOAT4X4 view = XMFLOAT4X4());
	void Draw(XMFLOAT4X4 view, XMFLOAT4X4 proj);

private:
	static DefaultCPUParticleShaders defaultShaders;

	void (*collisionCallback)(void* ptr) = nullptr;
	bool collisionsEnabled = false;

	bool blendingEnabled = true;

	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11BlendState* additiveBlend = nullptr;
	ID3D11DepthStencilState* depthWriteOff = nullptr;

	Particle* particlePool = nullptr;
	ParticlePhysics* particlePhysicsPool = nullptr;
	unsigned int* deadList = nullptr;
	unsigned int deadListCount = 0;
	ParticleVertex* particleVertices = nullptr;
	unsigned int* indices = nullptr;
	unsigned int particleVertCount = 0;
	unsigned int drawCount = 0;

	btGhostPairCallback* ghostCallback = nullptr;

	void InitBuffers();

	void CalcVertex(Particle p, XMFLOAT4X4 view);
};