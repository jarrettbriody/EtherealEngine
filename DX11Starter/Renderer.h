#pragma once
#include <DirectXMath.h>
#include "Vertex.h"
#include "Lights.h"
//#include "Shadows.h"
#include "SimpleShader.h"
#include "Entity.h"
#include "Camera.h"
#include <map>
#include <string>
#include <cmath>
#include <iostream>

using namespace std;

#define MAX_LIGHTS 32

class Renderer
{
private:
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* backBufferRTV;
	ID3D11DepthStencilView* depthStencilView;
	unsigned int viewPortWidth;
	unsigned int viewPortHeight;

	vector<Entity*>* entities = nullptr;
	Camera* camera = nullptr;

	map<string, Light*> lights;
	//map<string, Shadow> shadows;
	unsigned int lightCount = 0;
	bool shadowsEnabled = true;

	unsigned int shadowMapResolution = 2048;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11SamplerState* shadowSampler;
	ID3D11RasterizerState* shadowRasterizer;
	SimpleVertexShader* shadowVS = nullptr;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
public:
	Renderer(ID3D11Device* device, ID3D11DeviceContext*	context, IDXGISwapChain* swapChain, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, unsigned int viewPortWidth, unsigned int viewPortHeight);
	~Renderer();
	void InitShadows();
	void SetEntities(vector<Entity*>* entities);
	void SetCamera(Camera* camera);
	void SetShadowVertexShader(SimpleVertexShader* shadowVS);
	void ClearFrame();
	void RenderFrame();
	void PresentFrame();
	bool AddLight(std::string name, Light* newLight);
	bool RemoveLight(std::string name);
	void SendAllLightsToShader(SimplePixelShader* pixelShader);
	Light* GetLight(string name);
	void ToggleShadows(bool toggle);
	void SetShadowMapResolution(unsigned int res);
	void RenderShadowMap();
};

