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

// Order should match UpdateCS (RW binding issues)
RWStructuredBuffer<Particle>  ParticlePool		: register(u0);
ConsumeStructuredBuffer<uint> DeadParticles		: register(u1);
ConsumeStructuredBuffer<float> RandNums			: register(u2);

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	// Outside range?
	if ((id.x + 1) * (id.y + 1) > (uint)emitCount) return;

	// Grab a single index from the dead list
	uint newParticleIndex = DeadParticles.Consume();

	// Update it in the particle pool
	Particle newParticle = ParticlePool.Load(newParticleIndex);

	newParticle.color = float4(1, 0, 0, 1);

	// Color and position depend on the grid position and size
	//newParticle.color = float4(RandNums.Consume(), RandNums.Consume(), RandNums.Consume(), 1.0f);
	bool isColor = false;
	float colorRand = RandNums.Consume();
	for (int i = 0; i < colorCount; i++)
	{
		if (colorRand <= colors[i].weight) {
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
			if (colorRand <= textures[j].weight) {
				newParticle.textureIndex = textures[j].index;
				newParticle.transparency = textures[j].transparency;
				newParticle.originalTransparency = textures[j].transparency;
				break;
			}
		}
	}

	float speed = particleInitMinSpeed + (particleInitMaxSpeed - particleInitMinSpeed) * RandNums.Consume();
	int idOffset = (id.x + 6);
	int idOffset2 = (id.x + 7);
	float randomOffset = RandNums.Consume() * 2.0f - 1.0f;
	float randomOffset2 = RandNums.Consume() * 2.0f - 1.0f;

	float dirRand = RandNums.Consume();
	float3 start = normalize(float3(randomOffset, randomOffset2, 0.0f)) * (emissionStartRadius * dirRand);
	float3 end = float3(normalize(float2(randomOffset, randomOffset2)) * (emissionEndRadius * dirRand), 1.0f);

	newParticle.remainingLife = particleMinLifetime + (particleMaxLifetime - particleMinLifetime) * RandNums.Consume();
	newParticle.originalRemainingLife = newParticle.remainingLife;
	newParticle.position = start;
	newParticle.scale = particleInitMinScale + (particleInitMaxScale - particleInitMinScale) * RandNums.Consume();
	newParticle.velocity = normalize(end - start) * speed;
	newParticle.rotationRadians = 0.0f;
	newParticle.angularVelocity = particleInitMinAngularVelocity + (particleInitMaxAngularVelocity - particleInitMinAngularVelocity) * RandNums.Consume();
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