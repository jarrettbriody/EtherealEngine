#define MAX_PARTICLE_COLORS 16
#define MAX_PARTICLE_TEXTURES 8
#define MAX_RANDOM_NUMS_X4 32

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
	int textureIndex;
	float transparency;
	float originalTransparency;

	float originalRemainingLife;
	float3 padding;
};

struct ParticleColor {
	float4 color;
	float weight;
	float3 padding;
};

struct ParticleTexture {
	int index;
	float weight;
	float transparency;
	float padding;
};

struct ParticleDrawInfo
{
	uint index;
	float distanceSq;
};