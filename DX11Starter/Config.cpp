#include "Config.h"

ID3D11Device* Config::Device = nullptr;
ID3D11DeviceContext* Config::Context = nullptr;
ID3D11SamplerState* Config::Sampler = nullptr;

unsigned int Config::MemoryAllocatorSize = MEGABYTE * 512;
unsigned int Config::MemoryAllocatorAlignment = 16; //16 byte boundary
unsigned int Config::MemoryAllocatorEntityPoolSize = MEGABYTE * 128; //250 MB
unsigned int Config::MemoryAllocatorMeshPoolSize = MEGABYTE * 128; //250 MB
unsigned int Config::MemoryAllocatorMaterialPoolSize = MEGABYTE * 64; //250 MB