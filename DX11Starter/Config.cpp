#include "Config.h"

ID3D11Device* Config::Device = nullptr;
ID3D11DeviceContext* Config::Context = nullptr;
ID3D11SamplerState* Config::Sampler = nullptr;

unsigned int Config::MemoryAllocatorSize = MEGABYTE * 384;
unsigned int Config::MemoryAllocatorAlignment = 8; //8 byte boundary
unsigned int Config::MemoryAllocatorEntityPoolSize = MEGABYTE * 128; //250 MB
unsigned int Config::MemoryAllocatorMeshPoolSize = MEGABYTE * 128; //250 MB
unsigned int Config::MemoryAllocatorMaterialPoolSize = MEGABYTE * 128; //250 MB