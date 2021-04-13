#define MAX_PARTICLE_COLORS 16

struct Particle
{
	float4 color;

	float remainingLife;
	float3 position;

	float scale;
	float3 velocity;

	float3 acceleration;
	float rotationRadians;

	float angularVelocity;
	float3 padding;
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