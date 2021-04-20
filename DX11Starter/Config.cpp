#include "pch.h"
#include "Config.h"

ID3D11Device* Config::Device = nullptr;
ID3D11DeviceContext* Config::Context = nullptr;
ID3D11SamplerState* Config::Sampler = nullptr;
ID3D11SamplerState* Config::ClampSampler = nullptr;
IDXGISwapChain* Config::SwapChain = nullptr;
ID3D11RenderTargetView* Config::BackBufferRTV = nullptr;
ID3D11DepthStencilView* Config::DepthStencilView = nullptr;
unsigned int Config::ViewPortWidth = 1600;
unsigned int Config::ViewPortHeight = 900;
bool Config::Fullscreen = false;
bool Config::ShowCursor = true;
bool Config::DebugCamera = false;

unsigned int Config::MemoryAllocatorSize = MEGABYTE * 512;
unsigned int Config::MemoryAllocatorAlignment = 16; //16 byte boundary
unsigned int Config::MemoryAllocatorEntityPoolSize = MEGABYTE * 32; //128 MB
unsigned int Config::MemoryAllocatorMeshPoolSize = MEGABYTE * 128; //256 MB
unsigned int Config::MemoryAllocatorMaterialPoolSize = MEGABYTE * 32; //320 MB
unsigned int Config::MemoryAllocatorDecalPoolSize = MEGABYTE * 64; //384 MB
//unsigned int Config::MemoryAllocatorParticlePoolSize = MEGABYTE * 64; //448 MB

bool Config::EtherealDebugLinesEnabled = false;
bool Config::BulletDebugLinesEnabled = false;

bool Config::ShadowsEnabled = true;
bool Config::SSAOEnabled = false;
bool Config::HBAOPlusEnabled = true;

bool Config::DecalsEnabled = true;
unsigned int Config::InitialDecalVectorSize = 3000;

btDiscreteDynamicsWorld* Config::DynamicsWorld = nullptr;

unsigned int Config::SSAOSampleCount = 32;
float Config::SSAOKernelRadius = 1.0f;
DirectX::XMFLOAT4 Config::SSAOKernel[32];

std::map<std::string, unsigned int> Config::EntityTags = {
	{ "PLACEHOLDER", 1 },
};

std::map<std::string, unsigned int> Config::EntityLayers = {
	{ "decal", 1 },
};