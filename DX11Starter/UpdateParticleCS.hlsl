
#include "ParticleStructs.hlsli"

cbuffer ExternalData : register(b0)
{
	float deltaTime;
	float lifetime;
	float totalTime;
	int maxParticles;
	float fadeOutStartTime;
	float fadeInEndTime;
	int fadeOut;
	int fadeIn;
	float particleAvgLifetime;
	float3 cameraPos;
}

// Order should match EmitCS (RW binding issues)
RWStructuredBuffer<Particle>		 ParticlePool		: register(u0);
AppendStructuredBuffer<uint>		 DeadParticles		: register(u1);
RWStructuredBuffer<ParticleDrawInfo> ParticleDrawList	: register(u2);

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	// Valid particle?
	if ((id.x + 1) * (id.y + 1) > (uint)maxParticles) return;

	// Grab this particle
	Particle particle = ParticlePool.Load(id.x);

	// Early out for ALREADY DEAD particles (so they don't go back on dead list)
	if (particle.remainingLife <= 0.0f)	return;

	// Update the particle
	particle.remainingLife -= deltaTime;
	particle.velocity = particle.velocity + particle.acceleration * deltaTime;
	particle.position += particle.velocity * deltaTime;
	particle.rotationRadians += particle.angularVelocity * deltaTime;

	if (fadeIn == 1) {
		float totalLifetime = particle.originalRemainingLife - particle.remainingLife;
		if (fadeInEndTime > 0.0f && (totalLifetime < fadeInEndTime)) {
			particle.transparency = min((totalLifetime / fadeInEndTime), 1.0f) * particle.originalTransparency;
			particle.color.a = particle.transparency;
		}
	}

	if (fadeOut == 1) {
		if (fadeOutStartTime > 0.0f && (particle.remainingLife < fadeOutStartTime)) {
			particle.transparency = (particle.remainingLife / fadeOutStartTime) * particle.originalTransparency;
			particle.color.a = particle.transparency;
		}
		else if(fadeOutStartTime <= 0.0f){
			particle.transparency = (particle.remainingLife / particleAvgLifetime) * particle.originalTransparency;
			particle.color.a = particle.transparency;
		}
	}

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
		drawData.distanceSq = 0.0f;//dot(particle.position - cameraPos); // Not being used yet, but put here for future work

		ParticleDrawList[drawIndex] = drawData;
	}
}