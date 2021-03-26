#pragma once
#include "pch.h"
#include "ParticleEmitter.h"

struct DefaultGPUParticleShaders {
	SimpleComputeShader* initDeadListCS;
	SimpleComputeShader* copyDrawCountCS;
	SimpleComputeShader* particleEmissionCS;
	SimpleComputeShader* particleUpdateCS;
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
};

struct CustomGPUParticleShaders {
	SimpleComputeShader* particleEmissionCS;
	SimpleComputeShader* particleUpdateCS;
};

class GPUParticleEmitter : ParticleEmitter
{
public:
	static void SetDefaultShaders(DefaultGPUParticleShaders s);

	GPUParticleEmitter();
	GPUParticleEmitter(ParticleEmitterDescription d, bool blendingEnabled = false);
	~GPUParticleEmitter();

	void SetCustomShaders(SimpleComputeShader* emitParticle, SimpleComputeShader* updateParticle);
	void SetBlendingEnabled(bool toggle);

	void Update(float deltaTime, float totalTime);
	void Draw(XMFLOAT4X4 view, XMFLOAT4X4 proj);

private:
	static DefaultGPUParticleShaders defaultShaders;
	CustomGPUParticleShaders customShaders;
	bool customShadersEnabled = false;

	bool blendingEnabled = false;

	ID3D11Buffer* indexBuffer;
	ID3D11BlendState* additiveBlend;
	ID3D11DepthStencilState* depthWriteOff;

	ID3D11Buffer* drawArgsBuffer;

	ID3D11UnorderedAccessView* particlePoolUAV;
	ID3D11ShaderResourceView* particlePoolSRV;
	ID3D11UnorderedAccessView* particleDeadUAV;
	ID3D11UnorderedAccessView* particleDrawUAV;
	ID3D11ShaderResourceView* particleDrawSRV;
	ID3D11UnorderedAccessView* drawArgsUAV;

	float emitTimeCounter = 0.0f;

	ID3D11ShaderResourceView* none[16] = {};

	void InitBuffers();
};