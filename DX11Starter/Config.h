#pragma once
#include "pch.h"

//define macros
#define ZERO_VECTOR2 XMFLOAT2(0.0f,0.0f)
#define ZERO_VECTOR3 XMFLOAT3(0.0f,0.0f,0.0f)
#define ZERO_VECTOR4 XMFLOAT4(0.0f,0.0f,0.0f,0.0f)

#define ONE_VECTOR2 XMFLOAT2(1.0f,1.0f)
#define ONE_VECTOR3 XMFLOAT3(1.0f,1.0f,1.0f)
#define ONE_VECTOR4 XMFLOAT4(1.0f,1.0f,1.0f,1.0f)

#define X_AXIS XMFLOAT3(1.0f,0.0f,0.0f)
#define Y_AXIS XMFLOAT3(0.0f,1.0f,0.0f)
#define Z_AXIS XMFLOAT3(0.0f,0.0f,1.0f)
#define NEG_X_AXIS XMFLOAT3(-1.0f,0.0f,0.0f)
#define NEG_Y_AXIS XMFLOAT3(0.0f,-1.0f,0.0f)
#define NEG_Z_AXIS XMFLOAT3(0.0f,0.0f,-1.0f)

#define QUATERNION_IDENTITY XMFLOAT4(0, 0, 0, 1);
#define MATRIX_IDENTITY XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)

#define KILOBYTE 1024
#define MEGABYTE 1048576
#define GIGABYTE 1073741824

#define MAX_KERNEL_SAMPLES 32
#define MAX_DECALS_PER_ENTITY 512
#define MAX_PARTICLE_COLORS 16
#define MAX_PARTICLE_TEXTURES 8
#define MAX_SHADOW_CASCADES 4
#define MAX_LIGHTS 32
#define MAX_LIGHT_RENDER_DIST_SQ 75000
#define MAX_RANDOM_NUMS 128
#define MAX_POST_PROCESS_EFFECTS 8
#define MAX_ENTITY_TAG_COUNT 8
#define MAX_ENTITY_LAYER_COUNT 8
#define EESTRING_SIZE 64
#define MAX_UI_CALLBACKS 8
#define MAX_TRANSFORM_CHILDREN 1024

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
	LIGHT_POOL = 4,
	//PARTICLE_POOL = 4,
};

class Config
{
public:
	static HWND hWnd;

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
	static bool WindowedBorderless;
	static bool ShowCursor;
	static bool CaptureMouse;
	static bool DebugCamera; 
	static float DebugCameraSpeed;
	static float MouseSensitivity;

	static bool UpdatePaused;

	static unsigned int MemoryAllocatorSize; //1 gig ish
	static unsigned int MemoryAllocatorAlignment; //32 byte boundary
	static unsigned int MemoryAllocatorEntityPoolSize;
	static unsigned int MemoryAllocatorMeshPoolSize;
	static unsigned int MemoryAllocatorMaterialPoolSize;
	static unsigned int MemoryAllocatorDecalPoolSize;
	static unsigned int MemoryAllocatorLightPoolSize;
	//static unsigned int MemoryAllocatorParticlePoolSize;

	static bool EtherealDebugLinesEnabled;
	static bool BulletDebugLinesEnabled;

	static bool ShadowsEnabled;
	static bool SSAOEnabled;

	static bool HBAOPlusEnabled;
	static bool HBAOBlurEnabled;
	static float HBAOUnitsPerMeter;
	static float HBAORadius;
	static float HBAOPowerExponent;
	static float HBAOBlurSharpness;
	static GFSDK_SSAO_BlurRadius HBAOBlurRadius;

	static float SceneBrightness;
	static float SceneBrightnessMult;

	static bool DecalsEnabled;
	static unsigned int InitialDecalVectorSize;

	static float MusicVolume;
	static float SFXVolume;

	static btDiscreteDynamicsWorld* DynamicsWorld;
	
	static FMOD_RESULT FMODResult;
	static FMOD::System* FMODSystem;
	static FMOD::Sound* MainTheme[4];
	static FMOD::Sound* CombatTheme[3];

	static FMOD::Channel* SFXChannel;
	static FMOD::Channel* SFXChannel2D;
	static FMOD::ChannelGroup* SFXGroup;
	static FMOD::ChannelGroup* SFXGroup2D;

	static FMOD::Sound* Sword[9];
	static FMOD::Sound* Hookshot[5];
	static FMOD::Sound* Icicle[9];

	static FMOD::Sound* CyclopsAttack[7];
	static FMOD::Sound* CyclopsChatter[12];
	static FMOD::Sound* CyclopsDeath[6];

	static FMOD::Sound* BullAttack[7];
	static FMOD::Sound* BullChatter[12];
	static FMOD::Sound* BullDeath[6];

	static FMOD::Sound* HornedAttack[9];
	static FMOD::Sound* HornedChatter[6];
	static FMOD::Sound* HornedDeath[10];

	static FMOD::Sound* TowerAttack[9];
	static FMOD::Sound* TowerChatter[6];
	static FMOD::Sound* TowerDeath[10];

	static FMOD::Sound* Dash[6];
	static FMOD::Sound* Footstep; //Stone 10
	static FMOD::Sound* Jump[6];
	static FMOD::Sound* PlayerHit[10];

	static FMOD::Channel* MusicChannel;
	static FMOD::ChannelGroup* MasterGroup;
	static FMOD::ChannelGroup* MusicGroup;

	static unsigned int SSAOSampleCount;
	static float SSAOKernelRadius;
	static DirectX::XMFLOAT4 SSAOKernel[32];

	static std::map<std::string, unsigned int> EntityLayers;
};

