#include "Config.h"

ID3D11Device* Config::Device = nullptr;
ID3D11DeviceContext* Config::Context = nullptr;
ID3D11SamplerState* Config::Sampler = nullptr;
IDXGISwapChain* Config::SwapChain = nullptr;
ID3D11RenderTargetView* Config::BackBufferRTV = nullptr;
ID3D11DepthStencilView* Config::DepthStencilView = nullptr;
unsigned int Config::ViewPortWidth = 1600;
unsigned int Config::ViewPortHeight = 900;

unsigned int Config::MemoryAllocatorSize = MEGABYTE * 512;
unsigned int Config::MemoryAllocatorAlignment = 16; //16 byte boundary
unsigned int Config::MemoryAllocatorEntityPoolSize = MEGABYTE * 128; //250 MB
unsigned int Config::MemoryAllocatorMeshPoolSize = MEGABYTE * 128; //250 MB
unsigned int Config::MemoryAllocatorMaterialPoolSize = MEGABYTE * 64; //250 MB

bool Config::DebugLinesEnabled = true;

btDiscreteDynamicsWorld* Config::DynamicsWorld = nullptr;