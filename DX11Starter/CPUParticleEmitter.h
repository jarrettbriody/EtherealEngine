#pragma once
#include "pch.h"
#include "ParticleEmitter.h"
#include "MemoryAllocator.h"

struct DefaultCPUParticleShaders {
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
};

class CPUParticleEmitter : ParticleEmitter
{
public:
	static void SetDefaultShaders(DefaultCPUParticleShaders s);

	CPUParticleEmitter();
	CPUParticleEmitter(ParticleEmitterDescription d, bool blendingEnabled = true);
	~CPUParticleEmitter();

	void SetBlendingEnabled(bool toggle);

	void Update(float deltaTime, float totalTime, XMFLOAT4X4 view = XMFLOAT4X4());
	void Draw(XMFLOAT4X4 view, XMFLOAT4X4 proj);

private:
	static DefaultCPUParticleShaders defaultShaders;

	bool blendingEnabled = true;

	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	ID3D11BlendState* additiveBlend;
	ID3D11DepthStencilState* depthWriteOff;

	Particle* particlePool;
	unsigned int* deadList;
	unsigned int deadListCount;
	ParticleVertex* particleVertices;
	unsigned int particleVertCount;
	unsigned int drawCount;

	void InitBuffers();

	void CalcVertex(Particle p, XMFLOAT4X4 view);
};