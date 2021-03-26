#include "ParticleStructs.hlsli"

cbuffer ExternalData : register(b0)
{
	float totalTime;
	int emitCount;
	int maxParticles;
	float colorCount;

	float emissionAngleRadians; //angle of the cone the emitter will emit particles within [0.0f,360.0f]
	float particleMinLifetime; //minimum lifetime of emitted particles in seconds
	float particleMaxLifetime; //maximum lifetime of emitted particles in seconds
	float particleInitMinScale; //minimum initial scale of the particle
	float particleInitMaxScale; //maximum initial scale of the particle
	float particleInitMinAngularVelocity; //minimum initial angular velocity of the particle
	float particleInitMaxAngularVelocity; //maximum initial angular velocity of the particle
	float particleInitMinSpeed; //minimum initial speed of the particle
	float particleInitMaxSpeed; //maximum initial speed of the particle

	float2 padding;

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

	float random = sin(totalTime);

	newParticle.color = float4(1, 0, 0, 1);

	// Color and position depend on the grid position and size
	for (int i = colorCount - 1; i >= 0; i--)
	{
		if (random <= colors[i].weight) {
			newParticle.color = colors[i].color;
			break;
		}
	}
	
	float speed = particleInitMinSpeed + (particleInitMaxSpeed - particleInitMinSpeed) * random;
	float randomOffset = random * 2.0f - 1.0f;
	float angle = (emissionAngleRadians / 2.0f) * randomOffset;

	newParticle.remainingLife = particleMinLifetime + (particleMaxLifetime - particleMinLifetime) * random;
	newParticle.position = float3(0, 0, 0);
	newParticle.scale = particleInitMinScale + (particleInitMaxScale - particleInitMinScale) * random;
	newParticle.velocity = float3(cos(angle), sin(angle), 1.0f) * speed;
	newParticle.rotationRadians = 0.0f;
	newParticle.angularVelocity = particleInitMinAngularVelocity + (particleInitMaxAngularVelocity - particleInitMinAngularVelocity) * random;

	// Put it back
	ParticlePool[newParticleIndex] = newParticle;
}