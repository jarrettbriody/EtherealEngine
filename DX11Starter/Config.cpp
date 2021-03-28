#include "pch.h"
#include "Config.h"

ID3D11Device* Config::Device = nullptr;
ID3D11DeviceContext* Config::Context = nullptr;
ID3D11SamplerState* Config::Sampler = nullptr;
IDXGISwapChain* Config::SwapChain = nullptr;
ID3D11RenderTargetView* Config::BackBufferRTV = nullptr;
ID3D11DepthStencilView* Config::DepthStencilView = nullptr;
unsigned int Config::ViewPortWidth = 1280;
unsigned int Config::ViewPortHeight = 720;
bool Config::Fullscreen = false;
bool Config::FPSControllerEnabled = false;

unsigned int Config::MemoryAllocatorSize = MEGABYTE * 512;
unsigned int Config::MemoryAllocatorAlignment = 16; //16 byte boundary
unsigned int Config::MemoryAllocatorEntityPoolSize = MEGABYTE * 128; //250 MB
unsigned int Config::MemoryAllocatorMeshPoolSize = MEGABYTE * 128; //250 MB
unsigned int Config::MemoryAllocatorMaterialPoolSize = MEGABYTE * 64; //250 MB
unsigned int Config::MemoryAllocatorDecalPoolSize = MEGABYTE * 64;

bool Config::EtherealDebugLinesEnabled = false;
bool Config::BulletDebugLinesEnabled = false;

bool Config::ShadowsEnabled = true;
bool Config::SSAOEnabled = false;
bool Config::HBAOPlusEnabled = true;
bool Config::DecalsEnabled = true;

btDiscreteDynamicsWorld* Config::DynamicsWorld = nullptr;

unsigned int Config::SSAOSampleCount = 32;
float Config::SSAOKernelRadius = 1.0f;
DirectX::XMFLOAT4 Config::SSAOKernel[32];