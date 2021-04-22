#pragma once
#include "pch.h"
#include "Entity.h"

struct RendererShaders {
	SimpleVertexShader* depthStencilVS = nullptr;
	SimplePixelShader* depthStencilPS = nullptr;
	SimpleVertexShader* debugLineVS = nullptr;
	SimplePixelShader* debugLinePS = nullptr;
	SimpleVertexShader* decalVS = nullptr;
	SimplePixelShader* decalPS = nullptr;
	SimpleVertexShader* skyVS = nullptr;
	SimplePixelShader* skyPS = nullptr;
	SimpleVertexShader* postProcessVS = nullptr;
};

struct RendererCallback {
	void* data = nullptr;
	bool active = false;

	SimpleVertexShader* vShader = nullptr;
	SimplePixelShader* pShader = nullptr;

	SimpleVertexShader* prepassVShader = nullptr;
	SimplePixelShader* prepassPShader = nullptr;

	virtual void PreVertexShaderCallback() {};
	virtual void PrePixelShaderCallback() {};
	virtual void PrePrepassVertexShaderCallback() {};
	virtual void PrePrepassPixelShaderCallback() {};
};

struct RenderObject {
	Entity* entity;
	Mesh* mesh;
	Material* material;
	RendererCallback* callback = nullptr;
};

struct ShadowCascade {
	ID3D11DepthStencilView* DSV = nullptr;
	ID3D11ShaderResourceView* SRV = nullptr;
	unsigned int resolution = 2048;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;
	float width = 250.f;
	float height = 250.f;
	//float maxRange = 1000.0f;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT4X4 viewProj;
};

struct ShadowComponents {
	ShadowCascade shadowCascades[MAX_SHADOW_CASCADES];
	unsigned int cascadeCount = 1;
	ID3D11SamplerState* Sampler = nullptr;
	ID3D11RasterizerState* Rasterizer = nullptr;
	DirectX::XMFLOAT4X4 view;
	XMFLOAT3 sunPos;
};

struct DepthStencilComponents {
	ID3D11DepthStencilView* depthStencilDSV = nullptr;
	ID3D11RenderTargetView* depthStencilRTV = nullptr;
	ID3D11ShaderResourceView* depthStencilSRV = nullptr;
	ID3D11SamplerState* depthStencilSampler = nullptr;
	ID3D11RasterizerState* depthStencilRasterizer = nullptr;
	ID3D11DepthStencilState* depthStencilState = nullptr;

	ID3D11RenderTargetView* entityInfoRTV = nullptr;
	ID3D11ShaderResourceView* entityInfoSRV = nullptr;

	ID3D11BlendState* decalBlendState = nullptr;
};

struct HBAOPlusComponents {
	GFSDK_SSAO_CustomHeap CustomHeap;
	GFSDK_SSAO_Status status;
	GFSDK_SSAO_Context_D3D11* pAOContext = nullptr;
	GFSDK_SSAO_InputData_D3D11 Input;
	GFSDK_SSAO_Parameters_D3D11 Params;
};

struct SkyboxComponents {
	ID3D11ShaderResourceView* SRV = nullptr;
	ID3D11RasterizerState* Rasterizer = nullptr;
	ID3D11DepthStencilState* DepthStencilState = nullptr;
};

struct PostProcessComponents {
	bool enabled = false;
	ID3D11RenderTargetView* RTV = nullptr;
	ID3D11ShaderResourceView* SRV = nullptr;
	RendererCallback* callback = nullptr;
};