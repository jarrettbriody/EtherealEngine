
#include "ParticleStructs.hlsli"

cbuffer ExternalData : register(b0)
{
	float deltaTime;
	float lifetime;
	float totalTime;
	int maxParticles;
}

// Order should match EmitCS (RW binding issues)
RWStructuredBuffer<Particle>		 ParticlePool		: register(u0);
AppendStructuredBuffer<uint>		 DeadParticles		: register(u1);
RWStructuredBuffer<ParticleDrawInfo> ParticleDrawList	: register(u2);

[numthreads(32, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	// Valid particle?
	if (id.x >= (uint)maxParticles) return;

	// Grab this particle
	Particle particle = ParticlePool.Load(id.x);

	// Early out for ALREADY DEAD particles (so they don't go back on dead list)
	if (particle.remainingLife <= 0.0f)	return;

	// Update the particle
	particle.remainingLife -= deltaTime;
	particle.position += particle.velocity * deltaTime;
	particle.rotationRadians += particle.angularVelocity * deltaTime;

	// Put the particle back
	ParticlePool[id.x] = particle;

	// Newly dead?
	if (particle.remainingLife <= 0.0f)
	{
		// Add to dead list
		DeadParticles.Append(id.x);
	}
	else
	{
		// Increment the counter on the draw list, then put
		// the new draw data at the returned (pre-increment) index
		uint drawIndex = ParticleDrawList.IncrementCounter();

		// Set up draw data
		ParticleDrawInfo drawData;
		drawData.index = id.x; // This particle's actual index
		drawData.distanceSq = 0.0f; // Not being used yet, but put here for future work

		ParticleDrawList[drawIndex] = drawData;
	}
}