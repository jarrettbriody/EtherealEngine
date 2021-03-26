#define MAX_PARTICLE_COLORS 16

struct Particle
{
	float4 color;
	float remainingLife;
	float3 position;
	float scale;
	float3 velocity;
	float rotationRadians;
	float angularVelocity;
	float2 padding;
};

struct ParticleColor {
	float4 color;
	float weight;
	float3 padding;
};

struct ParticleDrawInfo
{
	uint index;
	float distanceSq;
};