#pragma once
#include "pch.h"

//define macros
#define ZERO_VECTOR3 XMFLOAT3(0.0f,0.0f,0.0f)
#define ONE_VECTOR3 XMFLOAT3(1.0f,1.0f,1.0f)
#define X_AXIS XMFLOAT3(1.0f,0.0f,0.0f)
#define Y_AXIS XMFLOAT3(0.0f,1.0f,0.0f)
#define Z_AXIS XMFLOAT3(0.0f,0.0f,1.0f)
#define NEG_X_AXIS XMFLOAT3(-1.0f,0.0f,0.0f)
#define NEG_Y_AXIS XMFLOAT3(0.0f,-1.0f,0.0f)
#define NEG_Z_AXIS XMFLOAT3(0.0f,0.0f,-1.0f)
#define MATRIX_IDENTITY XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
#define KILOBYTE 1024
#define MEGABYTE 1048576
#define GIGABYTE 1073741824
#define MAX_KERNEL_SAMPLES 32
#define MAX_DECALS_PER_ENTITY 128
#define MAX_PARTICLE_COLORS 16
#define MAX_PARTICLE_TEXTURES 8
#define MAX_SHADOW_CASCADES 4
#define MAX_LIGHTS 32

enum class MESH_TYPE {
	LOAD_FAILURE = -1,
	EMPTY_OBJECT = 0,
	DEFAULT_MESH = 1,
	GENERATED_MESH = 2,
};

enum class MEMORY_POOL {
	ENTITY_POOL = 0,
	MESH_POOL = 1,
	MATERIAL_POOL = 2,
	DECAL_POOL = 3,
	//PARTICLE_POOL = 4,
};

class Config
{
public:
	static ID3D11Device* Device;
	static ID3D11DeviceContext* Context;
	static ID3D11SamplerState* Sampler;
	static ID3D11SamplerState* ClampSampler;
	static IDXGISwapChain* SwapChain;
	static ID3D11RenderTargetView* BackBufferRTV;
	static ID3D11DepthStencilView* DepthStencilView;
	static unsigned int ViewPortWidth;
	static unsigned int ViewPortHeight;
	static bool Fullscreen;
	static bool ShowCursor;
	static bool DebugCamera;

	static unsigned int MemoryAllocatorSize; //1 gig ish
	static unsigned int MemoryAllocatorAlignment; //32 byte boundary
	static unsigned int MemoryAllocatorEntityPoolSize;
	static unsigned int MemoryAllocatorMeshPoolSize;
	static unsigned int MemoryAllocatorMaterialPoolSize;
	static unsigned int MemoryAllocatorDecalPoolSize;
	//static unsigned int MemoryAllocatorParticlePoolSize;

	static bool EtherealDebugLinesEnabled;
	static bool BulletDebugLinesEnabled;

	static bool ShadowsEnabled;
	static bool SSAOEnabled;
	static bool HBAOPlusEnabled;

	static bool DecalsEnabled;
	static unsigned int InitialDecalVectorSize;

	static btDiscreteDynamicsWorld* DynamicsWorld;

	static unsigned int SSAOSampleCount;
	static float SSAOKernelRadius;
	static DirectX::XMFLOAT4 SSAOKernel[32];

	static std::map<std::string, unsigned int> EntityTags;

	static std::map<std::string, unsigned int> EntityLayers;
};

