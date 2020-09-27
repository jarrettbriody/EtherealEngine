#pragma once
#include <d3d11.h>
class Config
{
public:
	static ID3D11Device* Device;
	static ID3D11DeviceContext* Context;
	static ID3D11SamplerState* Sampler;
};

