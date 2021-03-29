#pragma once
#include "pch.h"
#include "ParticleEmitter.h"
#include "MemoryAllocator.h"

class CPUParticleEmitter : ParticleEmitter
{
public:
	CPUParticleEmitter();
	CPUParticleEmitter(ParticleEmitterDescription d, bool blendingEnabled = true);
	~CPUParticleEmitter();

	void SetBlendingEnabled(bool toggle);

	void Update(float deltaTime, float totalTime);
	void Draw(XMFLOAT4X4 view, XMFLOAT4X4 proj);

private:
	bool blendingEnabled = true;

	ID3D11Buffer* indexBuffer;
	ID3D11BlendState* additiveBlend;
	ID3D11DepthStencilState* depthWriteOff;

	Particle* particlePool;
	unsigned int* drawList;
	unsigned int drawListCount;
	unsigned int* deadList;
	unsigned int deadListCount;

	float emitTimeCounter = 0.0f;

	ID3D11UnorderedAccessView* noneUAV[8] = {};
	ID3D11ShaderResourceView* none[16] = {};

	void InitBuffers();
};