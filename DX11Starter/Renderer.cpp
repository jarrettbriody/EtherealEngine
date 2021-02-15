#include "Renderer.h"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	shadowDSV->Release();
	shadowSRV->Release();
	shadowRasterizer->Release();
	shadowSampler->Release();

	map<string, Light*>::iterator lightMapIterator;
	for (int i = 0; i < lightCount; i++)
	{
		lightMapIterator = lights.begin();
		std::advance(lightMapIterator, i);

		if (lightMapIterator->second != nullptr)
		{
			delete lightMapIterator->second;
		}
	}

	map<string, Camera*>::iterator cameraMapIterator;
	for (int i = 0; i < cameraCount; i++)
	{
		cameraMapIterator = cameras.begin();
		std::advance(cameraMapIterator, i);

		if (cameraMapIterator->second != nullptr)
		{
			delete cameraMapIterator->second;
		}
	}

	for (size_t i = 0; i < DebugLines::debugLines.size(); i++)
	{
		delete DebugLines::debugLines[i];
	}

	delete[] renderObjects;
}

bool Renderer::SetupInstance()
{
	if (instance == nullptr) {
		instance = new Renderer();
		return true;
	}
	return false;
}

Renderer* Renderer::GetInstance()
{
	return instance;
}

bool Renderer::DestroyInstance()
{
	if (instance != nullptr) {
		delete instance;
		return true;
	}
	return false;
}

void Renderer::SetEntities(vector<Entity*>* entities)
{
	this->entities = entities;
	maxRenderObjects = entities->size() + 100;
	renderObjects = new RenderObject[maxRenderObjects];
}

void Renderer::SetShadowVertexShader(SimpleVertexShader * shadowVS)
{
	this->shadowVS = shadowVS;
}

void Renderer::SetDebugLineVertexShader(SimpleVertexShader* debugLineVS)
{
	this->debugLineVS = debugLineVS;
}

void Renderer::SetDebugLinePixelShader(SimplePixelShader* debugLinePS)
{
	this->debugLinePS = debugLinePS;
}

void Renderer::InitShadows()
{
	// Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapResolution;
	shadowDesc.Height = shadowMapResolution;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* shadowTexture;
	Config::Device->CreateTexture2D(&shadowDesc, 0, &shadowTexture);

	// Create the depth/stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	Config::Device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &shadowDSV);

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Config::Device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowSRV);

	// Release the texture reference since we don't need it
	shadowTexture->Release();

	// Create the special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	Config::Device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	Config::Device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	// Create the matrices that represent seeing the scene from
	// the light's point of view
	XMFLOAT3 dir = lights["Sun"]->Direction;
	XMMATRIX shadowView = XMMatrixTranspose(XMMatrixLookToLH(
		XMVectorSet(dir.x * -15, dir.y * -15, dir.z * -15, 0),
		XMVectorSet(dir.x, dir.y, dir.z, 0),
		XMVectorSet(0, 1, 0, 0)));
	XMStoreFloat4x4(&shadowViewMatrix, shadowView);

	XMMATRIX shadowProj = XMMatrixTranspose(XMMatrixOrthographicLH(
		45,
		45,
		0.1f,
		200));
	XMStoreFloat4x4(&shadowProjectionMatrix, shadowProj);
}

void Renderer::ToggleShadows(bool toggle)
{
	shadowsEnabled = toggle;
}

void Renderer::SetShadowMapResolution(unsigned int res)
{
	unsigned int exponent = (unsigned int)(log2((double)res) + 0.5);
	shadowMapResolution = (unsigned int)(pow(2u, exponent) + 0.5);
}

void Renderer::ClearFrame()
{
	// Background color
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	Config::Context->ClearRenderTargetView(Config::BackBufferRTV, color);
	Config::Context->ClearDepthStencilView(
		Config::DepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
}

void Renderer::RenderFrame()
{
	if (!entities || !camera || renderObjectCount == 0)
		return;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (int i = renderObjectCount - 1; i >= 0; i--)
	{
		RenderObject renderObject = renderObjects[i];
		Entity* e = renderObject.entity;
		Mesh* mesh = renderObject.mesh;
		Material* mat = renderObject.material;

		if (e->isEmptyObj) continue;
		e->ToggleShadows(shadowsEnabled);
		if (shadowsEnabled) {
			ShadowData d;
			d.shadowViewMatrix = shadowViewMatrix;
			d.shadowProjectionMatrix = shadowProjectionMatrix;
			d.shadowSRV = shadowSRV;
			d.shadowSampler = shadowSampler;
			e->SetShadowData(d);
		}
		
		ID3D11Buffer* vbo = mesh->GetVertexBuffer();
		ID3D11Buffer* ind = mesh->GetIndexBuffer();
		Config::Context->IASetVertexBuffers(0, 1, &vbo, &stride, &offset);
		Config::Context->IASetIndexBuffer(ind, DXGI_FORMAT_R32_UINT, 0);

		e->PrepareMaterialForDraw(mat->GetName(), camera->GetViewMatrix(), camera->GetProjMatrix());

		Config::Context->DrawIndexed(
			mesh->GetIndexCount(),		// The number of indices to use (we could draw a subset if we wanted)
			0,											// Offset to the first index we want to use
			0);											// Offset to add to each index when looking up vertices

		mat->GetPixelShader()->SetShaderResourceView("ShadowMap", NULL);
	}

	if (Config::DebugLinesEnabled) RenderDebugLines();
}

void Renderer::PresentFrame()
{
	Config::SwapChain->Present(0, 0);
	Config::Context->OMSetRenderTargets(1, &Config::BackBufferRTV, Config::DepthStencilView);
}

void Renderer::RenderDebugLines()
{
	Config::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	UINT stride = sizeof(DebugLinesVertex);
	UINT offset = 0;

	for (size_t i = 0; i < DebugLines::debugLines.size(); i++)
	{
		Config::Context->IASetVertexBuffers(0, 1, &DebugLines::debugLines[i]->vertexBuffer, &stride, &offset);
		Config::Context->IASetIndexBuffer(DebugLines::debugLines[i]->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		debugLineVS->SetMatrix4x4("world", DebugLines::debugLines[i]->worldMatrix);
		debugLineVS->SetMatrix4x4("view", camera->GetViewMatrix());
		debugLineVS->SetMatrix4x4("projection", camera->GetProjMatrix());

		debugLineVS->SetShader();
		debugLinePS->SetShader();

		debugLineVS->CopyAllBufferData();
		debugLinePS->CopyAllBufferData();

		Config::Context->DrawIndexed(
			DebugLines::debugLines[i]->indexCount,		// The number of indices to use (we could draw a subset if we wanted)
			0,											// Offset to the first index we want to use
			0);											// Offset to add to each index when looking up vertices
	}

	Config::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::RenderShadowMap()
{
	if (!entities || !shadowVS || !shadowsEnabled || renderObjectCount == 0)
		return;

	// Initial setup - No RTV necessary - Clear shadow map
	Config::Context->OMSetRenderTargets(0, 0, shadowDSV);
	Config::Context->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	Config::Context->RSSetState(shadowRasterizer);

	// SET A VIEWPORT!!!
	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)shadowMapResolution;
	vp.Height = (float)shadowMapResolution;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	Config::Context->RSSetViewports(1, &vp);

	// Set up the shaders
	shadowVS->SetShader();
	shadowVS->SetMatrix4x4("view", shadowViewMatrix);
	shadowVS->SetMatrix4x4("projection", shadowProjectionMatrix);

	Config::Context->PSSetShader(0, 0, 0); // Turns OFF the pixel shader

	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (int i = renderObjectCount - 1; i >= 0; i--)
	{
		RenderObject renderObject = renderObjects[i];
		Entity* e = renderObject.entity;
		Mesh* mesh = renderObject.mesh;

		if (e->destroyed) {
			if (i == renderObjectCount - 1) {
				renderObjectCount--;
			}
			else {
				renderObjects[i] = renderObjects[renderObjectCount - 1];
				renderObjectCount--;
			}
			continue;
		}

		if (e->isEmptyObj) continue;

		ID3D11Buffer* vb = mesh->GetVertexBuffer();
		ID3D11Buffer* ib = mesh->GetIndexBuffer();

		// Set buffers in the input assembler
		Config::Context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		Config::Context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);


		shadowVS->SetMatrix4x4("world", e->GetWorldMatrix());
		shadowVS->CopyAllBufferData();

		// Finally do the actual drawing
		Config::Context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}

	// Revert to original pipeline state
	Config::Context->OMSetRenderTargets(1, &Config::BackBufferRTV, Config::DepthStencilView);
	vp.Width = (float)Config::ViewPortWidth;
	vp.Height = (float)Config::ViewPortHeight;
	Config::Context->RSSetViewports(1, &vp);
	Config::Context->RSSetState(0);
}

bool Renderer::AddCamera(string name, Camera* newCamera)
{
	if (cameras.count(name)) {
		return false;
	}
	cameras.insert({ name, newCamera });
	cameraCount++;
	return true;
}

bool Renderer::RemoveCamera(string name)
{
	if (!cameras.count(name)) {
		return false;
	}
	cameras.erase(name);
	cameraCount--;
	return true;
}

Camera* Renderer::GetCamera(string name)
{
	if (!cameras.count(name)) {
		return nullptr;
	}
	return cameras[name];
}

bool Renderer::EnableCamera(string name)
{
	if (!cameras.count(name)) {
		return false;
	}
	camera = cameras[name];
	return true;
}

bool Renderer::AddLight(std::string name, Light* newLight)
{
	if (lights.count(name) || lightCount >= MAX_LIGHTS) {
		return false;
	}
	lights.insert({ name, newLight });
	lightCount++;
	return true;
}

bool Renderer::RemoveLight(std::string name)
{
	if (!lights.count(name)) {
		return false;
	}
	lights.erase(name);
	lightCount--;
	return true;
}

void Renderer::SendAllLightsToShader(SimplePixelShader* pixelShader)
{
	Light lightArray[MAX_LIGHTS];
	map<string, Light*>::iterator lightMapIterator;
	for (int i = 0; i < lightCount; i++)
	{
		lightMapIterator = lights.begin();
		std::advance(lightMapIterator, i);
		lightArray[i] = *lightMapIterator->second;
	}
	pixelShader->SetData(
		"lights",
		&lightArray,
		sizeof(lightArray)
	);
	pixelShader->SetData(
		"lightCount",
		&lightCount,
		sizeof(lightCount)
	);
	pixelShader->SetFloat3("cameraPosition", camera->position);
}

Light* Renderer::GetLight(string name)
{
	if (lights.count(name)) {
		return lights[name];
	}
	return {};
}

void Renderer::AddRenderObject(Entity* e, Mesh* mesh, Material* mat)
{
	RenderObject r;
	if (!mesh->HasChildren()) {
		r = { e, mesh, mat };
		renderObjects[renderObjectCount] = r;
		renderObjectCount++;
		if (renderObjectCount >= maxRenderObjects) {
			RenderObject* old = renderObjects;
			renderObjects = new RenderObject[(size_t)maxRenderObjects * 2];
			memcpy(renderObjects, old, sizeof(RenderObject) * maxRenderObjects);
			maxRenderObjects *= 2;
			delete[] old;
		}
	}
	else {
		Mesh** children = mesh->GetChildren();
		for (size_t i = 0; i < mesh->GetChildCount(); i++)
		{
			r = { e, children[i], e->GetMaterial(e->GetMeshMaterialName(i)) };
			renderObjects[renderObjectCount] = r;
			renderObjectCount++;
			if (renderObjectCount >= maxRenderObjects) {
				RenderObject* old = renderObjects;
				renderObjects = new RenderObject[(size_t)maxRenderObjects * 2];
				memcpy(renderObjects, old, sizeof(RenderObject) * maxRenderObjects);
				maxRenderObjects *= 2;
				delete[] old;
			}
		}
	}
}
