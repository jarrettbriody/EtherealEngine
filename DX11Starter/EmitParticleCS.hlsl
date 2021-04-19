#include "ParticleStructs.hlsli"

cbuffer ExternalData : register(b0)
{
	float totalTime;
	int emitCount;
	int maxParticles;
	int colorCount;

	float emissionStartRadius;
	float emissionEndRadius;
	float particleMinLifetime; //minimum lifetime of emitted particles in seconds
	float particleMaxLifetime; //maximum lifetime of emitted particles in seconds

	float particleInitMinScale; //minimum initial scale of the particle
	float particleInitMaxScale; //maximum initial scale of the particle
	float particleInitMinAngularVelocity; //minimum initial angular velocity of the particle
	float particleInitMaxAngularVelocity; //maximum initial angular velocity of the particle

	float particleInitMinSpeed; //minimum initial speed of the particle
	float particleInitMaxSpeed; //maximum initial speed of the particle
	float randomNum;
	float randomNum2;

	float3 particleAcceleration;
	float randomNum3;

	float randomNum4;
	float randomNum5;
	int textureCount;
	float padding;

	ParticleColor colors[MAX_PARTICLE_COLORS];
	ParticleTexture textures[MAX_PARTICLE_TEXTURES];
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

	newParticle.color = float4(1, 0, 0, 1);

	// Color and position depend on the grid position and size
	bool isColor = false;
	for (int i = 0; i < colorCount; i++)
	{
		if (randomNum3 <= colors[i].weight) {
			newParticle.color = colors[i].color;
			isColor = true;
			break;
		}
	}
	if (!isColor) {
		for (int j = 0; j < textureCount; j++)
		{
			if (randomNum3 <= textures[j].weight) {
				newParticle.textureIndex = j;
				break;
			}
		}
	}
	
	float speed = particleInitMinSpeed + (particleInitMaxSpeed - particleInitMinSpeed) * randomNum5;
	float randomOffset = randomNum * 2.0f - 1.0f;
	float randomOffset2 = randomNum2 * 2.0f - 1.0f;

	float3 start = float3(randomOffset, randomOffset2, 0.0f) * emissionStartRadius;
	float3 end = float3(float2(randomOffset, randomOffset2) * emissionEndRadius, 1.0f);

	newParticle.remainingLife = particleMinLifetime + (particleMaxLifetime - particleMinLifetime) * randomNum;
	newParticle.position = float3(0, 0, 0);
	newParticle.scale = particleInitMinScale + (particleInitMaxScale - particleInitMinScale) * randomNum4;
	newParticle.velocity = normalize(end - start) * speed;
	newParticle.rotationRadians = 0.0f;
	newParticle.angularVelocity = particleInitMinAngularVelocity + (particleInitMaxAngularVelocity - particleInitMinAngularVelocity) * randomNum;
	newParticle.acceleration = particleAcceleration;

	// Put it back
	ParticlePool[newParticleIndex] = newParticle;
}