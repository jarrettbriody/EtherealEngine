#pragma once
#include "pch.h"
class Config
{
public:
	static ID3D11Device* Device;
	static ID3D11DeviceContext* Context;
	static ID3D11SamplerState* Sampler;
	static IDXGISwapChain* SwapChain;
	static ID3D11RenderTargetView* BackBufferRTV;
	static ID3D11DepthStencilView* DepthStencilView;
	static unsigned int ViewPortWidth;
	static unsigned int ViewPortHeight;
	static bool Fullscreen;
	static bool FPSControllerEnabled;

	static unsigned int MemoryAllocatorSize; //1 gig ish
	static unsigned int MemoryAllocatorAlignment; //32 byte boundary
	static unsigned int MemoryAllocatorEntityPoolSize;
	static unsigned int MemoryAllocatorMeshPoolSize;
	static unsigned int MemoryAllocatorMaterialPoolSize;

	static bool EtherealDebugLinesEnabled;
	static bool BulletDebugLinesEnabled;

	static btDiscreteDynamicsWorld* DynamicsWorld;

	static unsigned int SSAOSampleCount;
	static float SSAOKernelRadius;
	static std::vector<DirectX::XMFLOAT4> SSAOKernel;

	static bool FORWARD;
	static bool BACKWARD;
	static bool LEFT;
	static bool RIGHT;
	static bool JUMP;
	static bool DASH;
};

