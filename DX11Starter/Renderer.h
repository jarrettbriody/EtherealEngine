#pragma once
#include <DirectXMath.h>
#include <map>
#include <string>
#include <cmath>
#include <iostream>
#include "Vertex.h"
#include "Lights.h"
#include "SimpleShader.h"
#include "Entity.h"
#include "Camera.h"
#include "DebugLines.h"

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
	map<string, Camera*> cameras;
	unsigned int cameraCount = 0;

	map<string, Light*> lights;
	//map<string, Shadow> shadows;
	unsigned int lightCount = 0;
	bool shadowsEnabled = true;

	bool debugLinesEnabled = true;

	unsigned int shadowMapResolution = 2048;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11SamplerState* shadowSampler;
	ID3D11RasterizerState* shadowRasterizer;
	SimpleVertexShader* shadowVS = nullptr;
	SimpleVertexShader* debugLineVS = nullptr;
	SimplePixelShader* debugLinePS = nullptr;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;
public:
	Renderer(ID3D11Device* device, ID3D11DeviceContext*	context, IDXGISwapChain* swapChain, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, unsigned int viewPortWidth, unsigned int viewPortHeight);
	~Renderer();
	void InitShadows();
	void SetEntities(vector<Entity*>* entities);
	void SetShadowVertexShader(SimpleVertexShader* shadowVS);
	void SetDebugLineVertexShader(SimpleVertexShader* debugLineVS);
	void SetDebugLinePixelShader(SimplePixelShader* debugLinePS);
	void ClearFrame();
	void RenderFrame();
	void PresentFrame();
	void RenderDebugLines();
	bool AddCamera(string name, Camera* newCamera);
	bool RemoveCamera(string name);
	Camera* GetCamera(string name);
	bool EnableCamera(string name);
	bool AddLight(std::string name, Light* newLight);
	bool RemoveLight(std::string name);
	void SendAllLightsToShader(SimplePixelShader* pixelShader);
	Light* GetLight(string name);
	void ToggleShadows(bool toggle);
	void SetShadowMapResolution(unsigned int res);
	void RenderShadowMap();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
};

