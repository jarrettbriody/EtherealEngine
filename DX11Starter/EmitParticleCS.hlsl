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
	int bakeWorldMat;
	int randNumCount;

	float3 particleAcceleration;
	int textureCount;

	matrix world;

	ParticleTexture textures[MAX_PARTICLE_TEXTURES];
}

cbuffer colorsBuffer : register(b1) {
	ParticleColor colors[MAX_PARTICLE_COLORS];
}

cbuffer randomNumbersBuffer : register(b2) {
	float4 randomNumbers[MAX_RANDOM_NUMS_X4];
}

// Order should match UpdateCS (RW binding issues)
RWStructuredBuffer<Particle>  ParticlePool		: register(u0);
ConsumeStructuredBuffer<uint> DeadParticles		: register(u1);

[numthreads(64, 1, 1)]
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
		if (randomNumbers[0][2] <= colors[i].weight) {
			newParticle.color = colors[i].color;
			newParticle.textureIndex = -1;
			newParticle.originalTransparency = colors[i].color.a;
			isColor = true;
			break;
		}
	}
	if (!isColor) {
		for (int j = 0; j < textureCount; j++)
		{
			if (randomNumbers[0][2] <= textures[j].weight) {
				newParticle.textureIndex = textures[j].index;
				newParticle.transparency = textures[j].transparency;
				newParticle.originalTransparency = textures[j].transparency;
				break;
			}
		}
	}

	float speed = particleInitMinSpeed + (particleInitMaxSpeed - particleInitMinSpeed) * randomNumbers[0][3];
	int idOffset = (id.x + 6);
	int idOffset2 = (id.x + 7);
	float randomOffset = randomNumbers[idOffset / 4][idOffset % 4] * 2.0f - 1.0f;
	float randomOffset2 = randomNumbers[idOffset2 / 4][idOffset2 % 4] * 2.0f - 1.0f;

	float3 start = normalize(float3(randomOffset, randomOffset2, 0.0f)) * (emissionStartRadius * randomNumbers[id.x / 4][id.x % 4]);
	float3 end = float3(normalize(float2(randomOffset, randomOffset2)) * (emissionEndRadius * randomNumbers[id.x / 4][id.x % 4]), 1.0f);

	newParticle.remainingLife = particleMinLifetime + (particleMaxLifetime - particleMinLifetime) * randomNumbers[0][0];
	newParticle.originalRemainingLife = newParticle.remainingLife;
	newParticle.position = start;
	newParticle.scale = particleInitMinScale + (particleInitMaxScale - particleInitMinScale) * randomNumbers[1][0];
	newParticle.velocity = normalize(end - start) * speed;
	newParticle.rotationRadians = 0.0f;
	newParticle.angularVelocity = particleInitMinAngularVelocity + (particleInitMaxAngularVelocity - particleInitMinAngularVelocity) * randomNumbers[0][1];
	newParticle.acceleration = particleAcceleration;
	newParticle.worldMatBaked = bakeWorldMat;
	if (bakeWorldMat == 1) {
		newParticle.position = mul(float4(newParticle.position, 1.0f), world);
		newParticle.velocity = mul(float4(newParticle.velocity, 0.0f), world);
		newParticle.acceleration = float4(newParticle.acceleration, 0.0f);
	}

	// Put it back
	ParticlePool[newParticleIndex] = newParticle;
}