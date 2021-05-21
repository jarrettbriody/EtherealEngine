#include "pch.h"
#include "Config.h"

HWND Config::hWnd;

ID3D11Device* Config::Device = nullptr;
ID3D11DeviceContext* Config::Context = nullptr;
ID3D11SamplerState* Config::Sampler = nullptr;
ID3D11SamplerState* Config::ClampSampler = nullptr;
IDXGISwapChain* Config::SwapChain = nullptr;
ID3D11RenderTargetView* Config::BackBufferRTV = nullptr;
ID3D11DepthStencilView* Config::DepthStencilView = nullptr;
unsigned int Config::ViewPortWidth = 1600;
unsigned int Config::ViewPortHeight = 900;
bool Config::Fullscreen = true;
bool Config::WindowedBorderless = false;
bool Config::ShowCursor = true;
bool Config::CaptureMouse = false;
bool Config::DebugCamera = false;
float Config::DebugCameraSpeed = 3.0f;
float Config::MouseSensitivity = 0.001f;

bool Config::UpdatePaused = false;

unsigned int Config::MemoryAllocatorSize = MEGABYTE * 1024;
unsigned int Config::MemoryAllocatorAlignment = 16; //16 byte boundary
unsigned int Config::MemoryAllocatorEntityPoolSize = MEGABYTE * 256; //128 MB
unsigned int Config::MemoryAllocatorMeshPoolSize = MEGABYTE * 128; //256 MB
unsigned int Config::MemoryAllocatorMaterialPoolSize = MEGABYTE * 32; //320 MB
unsigned int Config::MemoryAllocatorDecalPoolSize = MEGABYTE * 64; //384 MB
unsigned int Config::MemoryAllocatorLightPoolSize = MEGABYTE * 8;
//unsigned int Config::MemoryAllocatorParticlePoolSize = MEGABYTE * 64; //448 MB

bool Config::EtherealDebugLinesEnabled = false;
bool Config::BulletDebugLinesEnabled = false;

bool Config::ShadowsEnabled = true;
bool Config::SSAOEnabled = false;

bool Config::HBAOPlusEnabled = true;
bool Config::HBAOBlurEnabled = true;
float Config::HBAOUnitsPerMeter = 3.0f;
float Config::HBAORadius = 1.0f;
float Config::HBAOPowerExponent = 4.0f;
float Config::HBAOBlurSharpness = 8.0f;
GFSDK_SSAO_BlurRadius Config::HBAOBlurRadius = GFSDK_SSAO_BLUR_RADIUS_2;

float Config::SceneBrightness = 0.05f;
float Config::SceneBrightnessMult = 1.0f;

bool Config::DecalsEnabled = true;
unsigned int Config::InitialDecalVectorSize = 3000;

float Config::MusicVolume = 0.3f; //0.3
float Config::SFXVolume = 1.0f; //0.3

btDiscreteDynamicsWorld* Config::DynamicsWorld = nullptr;

FMOD_RESULT Config::FMODResult = FMOD_RESULT();
FMOD::System* Config::FMODSystem = nullptr;
FMOD::Sound* Config::MainTheme[4];
FMOD::Sound* Config::CombatTheme[3];
FMOD::Channel* Config::MusicChannel;
FMOD::ChannelGroup* Config::MasterGroup = nullptr;
FMOD::ChannelGroup* Config::MusicGroup = nullptr;

FMOD::Channel* Config::SFXChannel;
FMOD::Channel* Config::SFXChannel2D;
FMOD::ChannelGroup* Config::SFXGroup;
FMOD::ChannelGroup* Config::SFXGroup2D;

FMOD::Sound* Config::Sword[9];
FMOD::Sound* Config::Hookshot[5];
FMOD::Sound* Config::Icicle[9];
FMOD::Sound* Config::CyclopsAttack[7];
FMOD::Sound* Config::CyclopsChatter[12];
FMOD::Sound* Config::CyclopsDeath[6];

FMOD::Sound* Config::BullAttack[7];
FMOD::Sound* Config::BullChatter[12];
FMOD::Sound* Config::BullDeath[6];

FMOD::Sound* Config::HornedAttack[9];
FMOD::Sound* Config::HornedChatter[6];
FMOD::Sound* Config::HornedDeath[10];

FMOD::Sound* Config::TowerAttack[9];
FMOD::Sound* Config::TowerChatter[6];
FMOD::Sound* Config::TowerDeath[10];
			
FMOD::Sound* Config::Dash[6];
FMOD::Sound* Config::Footstep; //Stone 10
FMOD::Sound* Config::Jump[6];
FMOD::Sound* Config::PlayerHit[10];

unsigned int Config::SSAOSampleCount = 32;
float Config::SSAOKernelRadius = 1.0f;
DirectX::XMFLOAT4 Config::SSAOKernel[32];

std::map<std::string, unsigned int> Config::EntityLayers;