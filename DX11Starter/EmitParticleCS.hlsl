#include "ParticleStructs.hlsli"

cbuffer ExternalData : register(b0)
{
	float totalTime;
	int emitCount;
	int maxParticles;
	float colorCount;
	ParticleColor colors[MAX_PARTICLE_COLORS];

}

// Order should match UpdateCS (RW binding issues)
RWStructuredBuffer<Particle>  ParticlePool		: register(u0);
ConsumeStructuredBuffer<uint> DeadParticles		: register(u1);


[numthreads(32, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	// Outside range?
	if (id.x >= (uint)emitCount) return;

	// Grab a single index from the dead list
	uint newParticleIndex = DeadParticles.Consume();

	// Update it in the particle pool
	Particle newParticle = ParticlePool.Load(newParticleIndex);

	// Color and position depend on the grid position and size
	newParticle.color = float4(0.0f, 1.0f, 0.0f, 1);
	newParticle.remainingLife = 5.0f;
	newParticle.position = float3(0, 0, 0);
	newParticle.scale = 0.1f;
	newParticle.velocity = float3(0.0f, 0.0f, 1.0f);
	newParticle.rotationRadians = 0.0f;
	newParticle.angularVelocity = 0.0f;

	// Put it back
	ParticlePool[newParticleIndex] = newParticle;
}