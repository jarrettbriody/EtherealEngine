#include "pch.h"
#include "Renderer.h"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	for (size_t i = 0; i < shadowComponents.cascadeCount; i++)
	{
		if (shadowComponents.shadowCascades[i].shadowDSV)shadowComponents.shadowCascades[i].shadowDSV->Release();
		if (shadowComponents.shadowCascades[i].shadowSRV)shadowComponents.shadowCascades[i].shadowSRV->Release();
	}
	if(shadowComponents.shadowRasterizer) shadowComponents.shadowRasterizer->Release();
	if(shadowComponents.shadowSampler) shadowComponents.shadowSampler->Release();

	if(depthStencilComponents.depthStencilDSV) depthStencilComponents.depthStencilDSV->Release();
	if(depthStencilComponents.depthStencilSRV) depthStencilComponents.depthStencilSRV->Release();
	if(depthStencilComponents.depthStencilRTV) depthStencilComponents.depthStencilRTV->Release();
	if(depthStencilComponents.depthStencilRasterizer) depthStencilComponents.depthStencilRasterizer->Release();
	if(depthStencilComponents.depthStencilSampler) depthStencilComponents.depthStencilSampler->Release();

	if (depthStencilComponents.entityInfoSRV) depthStencilComponents.entityInfoSRV->Release();
	if (depthStencilComponents.entityInfoRTV) depthStencilComponents.entityInfoRTV->Release();

	if(depthStencilComponents.depthStencilState) depthStencilComponents.depthStencilState->Release();
	if(depthStencilComponents.decalBlendState) depthStencilComponents.decalBlendState->Release();

	if(skyboxComponents.skyDepthStencilState) skyboxComponents.skyDepthStencilState->Release();
	if(skyboxComponents.skyRasterizer) skyboxComponents.skyRasterizer->Release();

	blendState->Release();

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
	delete[] transparentObjects;

	if(hbaoPlusComponents.pAOContext)
		hbaoPlusComponents.pAOContext->Release();
}

void Renderer::CalcShadowMatrices(unsigned int cascadeIndex)
{
	// Create the matrices that represent seeing the scene from
	// the light's point of view
	XMFLOAT3 dir = lights["Sun"]->Direction;
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, XMVectorAdd(XMVectorAdd(XMLoadFloat3(&lights["Sun"]->Position), XMLoadFloat3(&camera->position)), XMVectorScale(XMLoadFloat3(&dir), -1.0f)));
	XMMATRIX shadowView = XMMatrixTranspose(XMMatrixLookToLH(
		XMVectorSet(pos.x,pos.y,pos.z, 0),
		XMVectorSet(dir.x, dir.y, dir.z, 0),
		XMVectorSet(0, 1, 0, 0)));
	XMStoreFloat4x4(&shadowComponents.shadowViewMatrix, shadowView);
	shadowComponents.sunPos = pos;

	XMMATRIX shadowProj = XMMatrixTranspose(XMMatrixOrthographicLH(
		shadowComponents.shadowCascades[cascadeIndex].width,
		shadowComponents.shadowCascades[cascadeIndex].height,
		shadowComponents.shadowCascades[cascadeIndex].nearPlane,
		shadowComponents.shadowCascades[cascadeIndex].farPlane));
	XMStoreFloat4x4(&shadowComponents.shadowCascades[cascadeIndex].shadowProjectionMatrix, shadowProj);

	XMStoreFloat4x4(&shadowComponents.shadowCascades[cascadeIndex].shadowViewProj, XMMatrixTranspose(XMMatrixMultiply(shadowView, shadowProj)));
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
	maxTransparentObjects = maxRenderObjects;
	renderObjects = new RenderObject[maxRenderObjects];
	transparentObjects = new RenderObject[maxTransparentObjects];
}

void Renderer::SetRendererShaders(RendererShaders rShaders)
{
	shaders = rShaders;
}

void Renderer::SetDecals(ID3D11ShaderResourceView* decals[8])
{
	for (size_t i = 0; i < 8; i++)
	{
		this->decals[i] = decals[i];
	}
}

void Renderer::SetMeshes(Mesh* cube, Mesh* invCube)
{
	this->cube = cube;
	this->invCube = invCube;
}

void Renderer::InitDepthStencil()
{
	if (depthStencilComponents.depthStencilDSV) depthStencilComponents.depthStencilDSV->Release();
	if (depthStencilComponents.depthStencilSRV) depthStencilComponents.depthStencilSRV->Release();
	if (depthStencilComponents.depthStencilRTV) depthStencilComponents.depthStencilRTV->Release();
	if (depthStencilComponents.depthStencilRasterizer) depthStencilComponents.depthStencilRasterizer->Release();
	if (depthStencilComponents.depthStencilSampler) depthStencilComponents.depthStencilSampler->Release();
	if (depthStencilComponents.entityInfoSRV) depthStencilComponents.entityInfoSRV->Release();
	if (depthStencilComponents.entityInfoRTV) depthStencilComponents.entityInfoRTV->Release();
	if (depthStencilComponents.depthStencilState) depthStencilComponents.depthStencilState->Release();
	if (depthStencilComponents.decalBlendState) depthStencilComponents.decalBlendState->Release();

	// Set up the texture itself
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.Format = DXGI_FORMAT_R32_FLOAT;
	texDesc.MipLevels = 1;
	texDesc.Height = Config::ViewPortHeight;
	texDesc.Width = Config::ViewPortWidth;
	texDesc.SampleDesc.Count = 1;

	// Actually create the texture
	ID3D11Texture2D* texture;
	Config::Device->CreateTexture2D(&texDesc, 0, &texture);

	// Create the shader resource view for this texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Config::Device->CreateShaderResourceView(texture, &srvDesc, &depthStencilComponents.depthStencilSRV);

	// Make a render target view desc and RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	Config::Device->CreateRenderTargetView(texture, &rtvDesc, &depthStencilComponents.depthStencilRTV);

	// Clean up extra texture ref
	texture->Release();

	// Set up the texture itself
	D3D11_TEXTURE2D_DESC entityTexDesc = {};
	entityTexDesc.ArraySize = 1;
	entityTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	entityTexDesc.Format = DXGI_FORMAT_R32_UINT;
	entityTexDesc.MipLevels = 1;
	entityTexDesc.Height = Config::ViewPortHeight;
	entityTexDesc.Width = Config::ViewPortWidth;
	entityTexDesc.SampleDesc.Count = 1;

	// Actually create the texture
	ID3D11Texture2D* entityTexture;
	Config::Device->CreateTexture2D(&entityTexDesc, 0, &entityTexture);

	// Create the shader resource view for this texture
	D3D11_SHADER_RESOURCE_VIEW_DESC entitySRVDesc = {};
	entitySRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	entitySRVDesc.Format = DXGI_FORMAT_R32_UINT;
	entitySRVDesc.Texture2D.MipLevels = 1;
	entitySRVDesc.Texture2D.MostDetailedMip = 0;
	Config::Device->CreateShaderResourceView(entityTexture, &entitySRVDesc, &depthStencilComponents.entityInfoSRV);

	// Make a render target view desc and RTV
	D3D11_RENDER_TARGET_VIEW_DESC entityRTVDesc = {};
	entityRTVDesc.Format = DXGI_FORMAT_R32_UINT;
	entityRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	entityRTVDesc.Texture2D.MipSlice = 0;
	Config::Device->CreateRenderTargetView(entityTexture, &entityRTVDesc, &depthStencilComponents.entityInfoRTV);

	// Clean up extra texture ref
	entityTexture->Release();

	D3D11_DEPTH_STENCIL_DESC dsDesc;

	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_NEVER;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ID3D11DepthStencilState* pDSState;
	Config::Device->CreateDepthStencilState(&dsDesc, &pDSState);

	depthStencilComponents.depthStencilState = pDSState;

	ID3D11BlendState* decalBlendState;

	D3D11_BLEND_DESC BlendState;

	BlendState.AlphaToCoverageEnable = false;
	BlendState.IndependentBlendEnable = false;
	BlendState.RenderTarget[0].BlendEnable = true;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	Config::Device->CreateBlendState(&BlendState, &decalBlendState);

	depthStencilComponents.decalBlendState = decalBlendState;
}

void Renderer::InitHBAOPlus()
{
	if (hbaoPlusComponents.pAOContext)
		hbaoPlusComponents.pAOContext->Release();

	//(1.) INITIALIZE THE LIBRARY

	hbaoPlusComponents.CustomHeap.new_ = ::operator new;
	hbaoPlusComponents.CustomHeap.delete_ = ::operator delete;

	hbaoPlusComponents.status = GFSDK_SSAO_CreateContext_D3D11(Config::Device, &hbaoPlusComponents.pAOContext, &hbaoPlusComponents.CustomHeap);
	assert(hbaoPlusComponents.status == GFSDK_SSAO_OK); // HBAO+ requires feature level 11_0 or above

	//(2.) SET INPUT DEPTHS

	hbaoPlusComponents.Input.DepthData.DepthTextureType = GFSDK_SSAO_VIEW_DEPTHS;//GFSDK_SSAO_HARDWARE_DEPTHS;
	hbaoPlusComponents.Input.DepthData.pFullResDepthTextureSRV = depthStencilComponents.depthStencilSRV;
	XMFLOAT4X4 proj = camera->GetProjMatrix();
	float mat[16];
	memcpy(mat, proj.m, sizeof(float) * 16);
	hbaoPlusComponents.Input.DepthData.ProjectionMatrix.Data = GFSDK_SSAO_Float4x4(mat);
	hbaoPlusComponents.Input.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_COLUMN_MAJOR_ORDER;
	hbaoPlusComponents.Input.DepthData.MetersToViewSpaceUnits = 0.5f;

	//(3.) SET AO PARAMETERS

	hbaoPlusComponents.Params.Radius = 4.f; // 2
	hbaoPlusComponents.Params.Bias = 0.1f;
	hbaoPlusComponents.Params.PowerExponent = 2.f;
	hbaoPlusComponents.Params.Blur.Enable = true;
	//hbaoPlusComponents.Params.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_8;
	hbaoPlusComponents.Params.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_2;
	hbaoPlusComponents.Params.Blur.Sharpness = 8.f; //4
	hbaoPlusComponents.Params.Output.BlendMode = GFSDK_SSAO_MULTIPLY_RGB;
	hbaoPlusComponents.Params.DetailAO = 1.0f;
}

void Renderer::InitShadows(unsigned int cascadeCount)
{
	shadowComponents.cascadeCount = cascadeCount;
	for (size_t i = 0; i < cascadeCount; i++)
	{
		// Create the actual texture that will be the shadow map
		D3D11_TEXTURE2D_DESC shadowDesc = {};
		shadowDesc.Width = shadowComponents.shadowCascades[i].shadowMapResolution;
		shadowDesc.Height = shadowComponents.shadowCascades[i].shadowMapResolution;
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
		Config::Device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &shadowComponents.shadowCascades[i].shadowDSV);

		// Create the SRV for the shadow map
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		Config::Device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowComponents.shadowCascades[i].shadowSRV);

		// Release the texture reference since we don't need it
		shadowTexture->Release();

		CalcShadowMatrices(i);
	}

	// Create the special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	//shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
	shadowSampDesc.MaxAnisotropy = 16;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	Config::Device->CreateSamplerState(&shadowSampDesc, &shadowComponents.shadowSampler);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	Config::Device->CreateRasterizerState(&shadowRastDesc, &shadowComponents.shadowRasterizer);
}

void Renderer::InitSkybox()
{
	D3D11_RASTERIZER_DESC skyRD = {};
	skyRD.CullMode = D3D11_CULL_FRONT;
	skyRD.FillMode = D3D11_FILL_SOLID;
	skyRD.DepthClipEnable = true;
	Config::Device->CreateRasterizerState(&skyRD, &skyboxComponents.skyRasterizer);

	D3D11_DEPTH_STENCIL_DESC skyDS = {};
	skyDS.DepthEnable = true;
	skyDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	skyDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	Config::Device->CreateDepthStencilState(&skyDS, &skyboxComponents.skyDepthStencilState);
}

void Renderer::SetSkybox(ID3D11ShaderResourceView* srv)
{
	skyboxComponents.skySRV = srv;
}

void Renderer::SetShadowCascadeInfo(unsigned int cascadeIndex, unsigned int resolution, float nearPlane, float farPlane, float width, float height, float maxRange)
{
	unsigned int exponent = (unsigned int)(log2((double)resolution) + 0.5);
	shadowComponents.shadowCascades[cascadeIndex].shadowMapResolution = (unsigned int)(pow(2u, exponent) + 0.5);
	shadowComponents.shadowCascades[cascadeIndex].nearPlane = nearPlane;
	shadowComponents.shadowCascades[cascadeIndex].farPlane = farPlane;
	shadowComponents.shadowCascades[cascadeIndex].width = width;
	shadowComponents.shadowCascades[cascadeIndex].height = height;
	shadowComponents.shadowCascades[cascadeIndex].maxRange = maxRange;
}

void Renderer::InitBlendState()
{
	D3D11_BLEND_DESC bd = {};
	bd.RenderTarget[0].BlendEnable = true;

	// These control how the RGB channels are combined
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	// These control how the alpha channel is combined
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	Config::Device->CreateBlendState(&bd, &blendState);
}

void Renderer::ToggleBlendState(bool toggle)
{
	if (toggle)
		Config::Context->OMSetBlendState(blendState, 0, 0xFFFFFFFF);
	else
		Config::Context->OMSetBlendState(NULL, 0, 0xFFFFFFFF);
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

	ToggleBlendState(false);

	for (int i = renderObjectCount - 1; i >= 0; i--)
	{
		RenderObject renderObject = renderObjects[i];
		Entity* e = renderObject.entity;
		Mesh* mesh = renderObject.mesh;
		Material* mat = renderObject.material;
		RendererCallback* callback = renderObject.callback;

		if (e->isEmptyObj) continue;
		e->ToggleShadows(Config::ShadowsEnabled);
		if (Config::ShadowsEnabled) {
			ShadowData d;
			for (size_t i = 0; i < MAX_SHADOW_CASCADES; i++)
			{
				d.shadowProjectionMatrix[i] = shadowComponents.shadowCascades[i].shadowProjectionMatrix;
				d.shadowSRV[i] = shadowComponents.shadowCascades[i].shadowSRV;
				d.nears[i] = shadowComponents.shadowCascades[i].nearPlane;
				d.fars[i] = shadowComponents.shadowCascades[i].farPlane;
				d.range[i] = shadowComponents.shadowCascades[i].maxRange;
			}
			d.sunPos = shadowComponents.sunPos;
			d.shadowViewMatrix = shadowComponents.shadowViewMatrix;
			d.shadowSampler = shadowComponents.shadowSampler;
			d.cascadeCount = shadowComponents.cascadeCount;
			e->SetShadowData(d);
		}

		if (Config::SSAOEnabled) {
			DepthStencilData d;
			d.depthStencilSRV = depthStencilComponents.depthStencilSRV;
			d.depthStencilSampler = depthStencilComponents.depthStencilSampler;
			e->SetDepthStencilData(d);
		}

		ID3D11Buffer* vbo = mesh->GetVertexBuffer();
		ID3D11Buffer* ind = mesh->GetIndexBuffer();
		Config::Context->IASetVertexBuffers(0, 1, &vbo, &stride, &offset);
		Config::Context->IASetIndexBuffer(ind, DXGI_FORMAT_R32_UINT, 0);

		if (callback != nullptr) {
			if (callback->active && callback->vShader) {
				callback->PreVertexShaderCallback();
			}
		}
		e->PrepareMaterialForDraw(mat->GetName(), camera->GetViewMatrix(), camera->GetProjMatrix());

		Config::Context->DrawIndexed(
			mesh->GetIndexCount(),		// The number of indices to use (we could draw a subset if we wanted)
			0,											// Offset to the first index we want to use
			0);											// Offset to add to each index when looking up vertices

		mat->GetPixelShader()->SetShaderResourceView("ShadowMap0", NULL);
		mat->GetPixelShader()->SetShaderResourceView("ShadowMap1", NULL);
		mat->GetPixelShader()->SetShaderResourceView("ShadowMap2", NULL);
		mat->GetPixelShader()->SetShaderResourceView("ShadowMap3", NULL);
	}

	if (Config::DecalsEnabled) {
		//ID3D11BlendState* originalBlend;
		//Config::Context->OMGetBlendState(&originalBlend);
		Config::Context->OMSetDepthStencilState(depthStencilComponents.depthStencilState, 0);
		Config::Context->OMSetBlendState(depthStencilComponents.decalBlendState, 0, 0xFFFFFFFF);
		ID3D11Buffer* vbo = invCube->GetVertexBuffer();
		ID3D11Buffer* ind = invCube->GetIndexBuffer();
		Config::Context->IASetVertexBuffers(0, 1, &vbo, &stride, &offset);
		Config::Context->IASetIndexBuffer(ind, DXGI_FORMAT_R32_UINT, 0);

		shaders.decalVS->SetShader();
		shaders.decalPS->SetShader();

		shaders.decalVS->SetFloat3("cameraPos", camera->position);

		//shaders.decalPS->SetMatrix4x4("shadowViewProj", shadowComponents.shadowViewProj);
		shaders.decalPS->SetMatrix4x4("shadowView", shadowComponents.shadowViewMatrix);

		shaders.decalPS->SetFloat3("sunPos", shadowComponents.sunPos);
		shaders.decalPS->SetShaderResourceView("DepthBuffer", depthStencilComponents.depthStencilSRV);
		shaders.decalPS->SetShaderResourceView("EntityInfoBuffer", depthStencilComponents.entityInfoSRV);

		for (size_t i = 0; i < MAX_SHADOW_CASCADES; i++)
		{
			string str = to_string(i);
			shaders.decalPS->SetShaderResourceView("ShadowMap" + str, shadowComponents.shadowCascades[i].shadowSRV);
			shaders.decalPS->SetMatrix4x4("shadowProj" + str, shadowComponents.shadowCascades[i].shadowProjectionMatrix);
			shaders.decalPS->SetFloat("cascadeRange" + str, shadowComponents.shadowCascades[i].maxRange);
		}

		shaders.decalPS->SetSamplerState("ShadowSampler", shadowComponents.shadowSampler);
		shaders.decalPS->SetFloat3("cameraPos", camera->position);

		XMFLOAT4X4 world;
		XMFLOAT4X4 invWorld;
		XMMATRIX ownerWorld;
		XMMATRIX localWorld;
		XMMATRIX cWorld;
		for (size_t i = 0; i < DecalHandler::decalsVec.size(); i++)
		{
			DecalBucket* db = DecalHandler::decalsVec[i];
			ownerWorld = XMMatrixTranspose(XMLoadFloat4x4(&db->owner->GetWorldMatrix()));
			for (size_t j = 0; j < db->count; j++)
			{
				localWorld = XMMatrixTranspose(XMLoadFloat4x4(&db->decals[j].localTransform));
				cWorld = XMMatrixMultiply(localWorld, ownerWorld);
				XMStoreFloat4x4(&world, XMMatrixTranspose(cWorld));
				XMStoreFloat4x4(&invWorld, XMMatrixTranspose(XMMatrixInverse(nullptr, cWorld)));

				shaders.decalVS->SetMatrix4x4("world", world);
				shaders.decalVS->SetMatrix4x4("view", camera->GetViewMatrix());
				shaders.decalVS->SetMatrix4x4("projection", camera->GetProjMatrix());
				//shaders.decalVS->SetMatrix4x4("shadowView", shadowComponents.shadowViewMatrix);
				//shaders.decalVS->SetMatrix4x4("shadowProj", shadowComponents.shadowProjectionMatrix);
				//Config::Context->PSSetShaderResources(2, 8, decals);
				shaders.decalPS->SetShaderResourceView("Decal", decals[db->decals[j].type]);
				shaders.decalPS->SetMatrix4x4("worldMatrix", world);
				shaders.decalPS->SetMatrix4x4("inverseWorldMatrix", invWorld);

				shaders.decalVS->CopyAllBufferData();
				shaders.decalPS->CopyAllBufferData();

				Config::Context->DrawIndexed(
					invCube->GetIndexCount(),						// The number of indices to use (we could draw a subset if we wanted)
					0,											// Offset to the first index we want to use
					0);											// Offset to add to each index when looking up vertices
			}
		}
		shaders.decalPS->SetShaderResourceView("ShadowMap0", NULL);
		shaders.decalPS->SetShaderResourceView("ShadowMap1", NULL);
		shaders.decalPS->SetShaderResourceView("ShadowMap2", NULL);
		shaders.decalPS->SetShaderResourceView("ShadowMap3", NULL);
		shaders.decalPS->SetShaderResourceView("DepthBuffer", NULL);
		shaders.decalPS->SetShaderResourceView("EntityInfoBuffer", NULL);
		Config::Context->OMSetDepthStencilState(NULL, 0);
		Config::Context->OMSetBlendState(NULL, 0, 0xFFFFFFFF);
	}

	ToggleBlendState(true);

	stride = sizeof(Vertex);
	offset = 0;

	for (int i = transparentObjectCount - 1; i >= 0; i--)
	{
		RenderObject renderObject = transparentObjects[i];
		Entity* e = renderObject.entity;
		Mesh* mesh = renderObject.mesh;
		Material* mat = renderObject.material;

		if (e->isEmptyObj) continue;

		if (Config::SSAOEnabled) {
			DepthStencilData d;
			d.depthStencilSRV = depthStencilComponents.depthStencilSRV;
			d.depthStencilSampler = depthStencilComponents.depthStencilSampler;
			e->SetDepthStencilData(d);
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
	}

	if (Config::HBAOPlusEnabled) {
		//(4.) RENDER AO

		hbaoPlusComponents.status = hbaoPlusComponents.pAOContext->RenderAO(Config::Context, &hbaoPlusComponents.Input, &hbaoPlusComponents.Params, Config::BackBufferRTV);
		assert(hbaoPlusComponents.status == GFSDK_SSAO_OK);
	}

	/*if (Config::EtherealDebugLinesEnabled)*/ RenderDebugLines();
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

		shaders.debugLineVS->SetMatrix4x4("world", DebugLines::debugLines[i]->worldMatrix);
		shaders.debugLineVS->SetMatrix4x4("view", camera->GetViewMatrix());
		shaders.debugLineVS->SetMatrix4x4("projection", camera->GetProjMatrix());

		shaders.debugLineVS->SetShader();
		shaders.debugLinePS->SetShader();

		shaders.debugLineVS->CopyAllBufferData();
		shaders.debugLinePS->CopyAllBufferData();

		Config::Context->DrawIndexed(
			DebugLines::debugLines[i]->indexCount,		// The number of indices to use (we could draw a subset if we wanted)
			0,											// Offset to the first index we want to use
			0);											// Offset to add to each index when looking up vertices
	}

	Config::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::RenderShadowMap()
{
	if (!entities || !shaders.depthStencilVS || !Config::ShadowsEnabled || renderObjectCount == 0)
		return;

	D3D11_VIEWPORT vp = {};

	for (size_t i = 0; i < shadowComponents.cascadeCount; i++)
	{
		// Initial setup - No RTV necessary - Clear shadow map
		Config::Context->OMSetRenderTargets(0, 0, shadowComponents.shadowCascades[i].shadowDSV);
		Config::Context->ClearDepthStencilView(shadowComponents.shadowCascades[i].shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
		Config::Context->RSSetState(shadowComponents.shadowRasterizer);

		// SET A VIEWPORT!!!
		vp = {};
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)shadowComponents.shadowCascades[i].shadowMapResolution;
		vp.Height = (float)shadowComponents.shadowCascades[i].shadowMapResolution;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		Config::Context->RSSetViewports(1, &vp);

		CalcShadowMatrices();

		// Set up the shaders
		shaders.depthStencilVS->SetShader();
		shaders.depthStencilVS->SetMatrix4x4("view", shadowComponents.shadowViewMatrix);
		shaders.depthStencilVS->SetMatrix4x4("projection", shadowComponents.shadowCascades[i].shadowProjectionMatrix);
		//shaders.depthStencilPS->SetFloat3("cameraPosition", lights["Sun"]->Position);
		//shaders.depthStencilPS->CopyAllBufferData();

		Config::Context->PSSetShader(0, 0, 0); // Turns OFF the pixel shader

		// Set buffers in the input assembler
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		for (int i = renderObjectCount - 1; i >= 0; i--)
		{
			shaders.depthStencilVS->SetShader();

			RenderObject renderObject = renderObjects[i];
			Entity* e = renderObject.entity;
			Mesh* mesh = renderObject.mesh;
			Material* mat = renderObject.material;
			RendererCallback* callback = renderObject.callback;

			if (e->destroyed || e->isEmptyObj) {
				if (i != renderObjectCount - 1) {
					renderObjects[i] = renderObjects[renderObjectCount - 1];
				}
				renderObjectsMap.erase(e);
				renderObjectCount--;
				continue;
			}

			if (e->isEmptyObj) continue;

			//if (mat->GetVertexShader()->GetShaderType() == ShaderType::MODIFY_VERTS) {}

			ID3D11Buffer* vb = mesh->GetVertexBuffer();
			ID3D11Buffer* ib = mesh->GetIndexBuffer();

			// Set buffers in the input assembler
			Config::Context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
			Config::Context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

			if (callback != nullptr) {
				if (callback->active && callback->prepassVShader) {
					callback->prepassVShader->SetShader();
					callback->prepassVShader->SetMatrix4x4("world", e->GetWorldMatrix());
					callback->prepassVShader->SetMatrix4x4("view", shadowComponents.shadowViewMatrix);
					callback->prepassVShader->SetMatrix4x4("projection", shadowComponents.shadowCascades[i].shadowProjectionMatrix);
					callback->PrePrepassVertexShaderCallback();
					callback->prepassVShader->CopyAllBufferData();
				}
				else {
					shaders.depthStencilVS->SetMatrix4x4("world", e->GetWorldMatrix());
					shaders.depthStencilVS->CopyAllBufferData();
				}
			}
			else {
				shaders.depthStencilVS->SetMatrix4x4("world", e->GetWorldMatrix());
				shaders.depthStencilVS->CopyAllBufferData();
			}

			// Finally do the actual drawing
			Config::Context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
		}
	}
	
	// Revert to original pipeline state
	Config::Context->OMSetRenderTargets(1, &Config::BackBufferRTV, Config::DepthStencilView);
	vp.Width = (float)Config::ViewPortWidth;
	vp.Height = (float)Config::ViewPortHeight;
	Config::Context->RSSetViewports(1, &vp);
	Config::Context->RSSetState(0);
}

void Renderer::RenderDepthStencil()
{
	if (!entities || !shaders.depthStencilVS || (!Config::SSAOEnabled && !Config::HBAOPlusEnabled) || renderObjectCount == 0)
		return;

	// Initial setup - No RTV necessary - Clear depthStencil map
	//Config::Context->OMSetRenderTargets(0, 0, depthStencilComponents.depthStencilDSV);

	ID3D11RenderTargetView* targets[2] = {
		depthStencilComponents.depthStencilRTV,
		depthStencilComponents.entityInfoRTV,
	};
	Config::Context->OMSetRenderTargets(2, targets, Config::DepthStencilView);

	//Config::Context->OMSetRenderTargets(1, &depthStencilComponents.depthStencilRTV, Config::DepthStencilView);
	//Config::Context->ClearDepthStencilView(depthStencilComponents.depthStencilDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Config::Context->ClearRenderTargetView(depthStencilComponents.depthStencilRTV, color);
	Config::Context->ClearRenderTargetView(depthStencilComponents.entityInfoRTV, color);

	//Config::Context->RSSetState(depthStencilComponents.depthStencilRasterizer);

	// Set up the shaders
	shaders.depthStencilVS->SetShader();
	shaders.depthStencilPS->SetShader();
	shaders.depthStencilVS->SetMatrix4x4("view", camera->GetViewMatrix());
	shaders.depthStencilVS->SetMatrix4x4("projection", camera->GetProjMatrix());
	shaders.depthStencilPS->SetFloat3("cameraPosition", camera->position);

	//Config::Context->PSSetShader(0, 0, 0); // Turns OFF the pixel shader

	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	unsigned int entityInfo;

	for (int i = renderObjectCount - 1; i >= 0; i--)
	{
		shaders.depthStencilVS->SetShader();

		RenderObject renderObject = renderObjects[i];
		Entity* e = renderObject.entity;
		Mesh* mesh = renderObject.mesh;
		Material* mat = renderObject.material;
		RendererCallback* callback = renderObject.callback;

		if (e->isEmptyObj || !renderObject.material->GetMaterialData().hbaoPlusEnabled) continue;

		//if (mat->GetVertexShader()->GetShaderType() == ShaderType::MODIFY_VERTS) {}

		ID3D11Buffer* vb = mesh->GetVertexBuffer();
		ID3D11Buffer* ib = mesh->GetIndexBuffer();

		// Set buffers in the input assembler
		Config::Context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		Config::Context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		if (callback != nullptr) {
			if (callback->active && callback->prepassVShader) {
				callback->prepassVShader->SetShader();
				callback->prepassVShader->SetMatrix4x4("world", e->GetWorldMatrix());
				callback->prepassVShader->SetMatrix4x4("view", camera->GetViewMatrix());
				callback->prepassVShader->SetMatrix4x4("projection", camera->GetProjMatrix());
				callback->PrePrepassVertexShaderCallback();
				callback->prepassVShader->CopyAllBufferData();
			}
			else {
				shaders.depthStencilVS->SetMatrix4x4("world", e->GetWorldMatrix());
				shaders.depthStencilVS->CopyAllBufferData();
			}
		}
		else {
			shaders.depthStencilVS->SetMatrix4x4("world", e->GetWorldMatrix());
			shaders.depthStencilVS->CopyAllBufferData();
		}

		entityInfo = 0;
		entityInfo = Config::EntityLayers[e->layer.STDStr()];
		shaders.depthStencilPS->SetInt("entityInfo", entityInfo);
		shaders.depthStencilPS->CopyAllBufferData();

		// Finally do the actual drawing
		Config::Context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}

	// Revert to original pipeline state

	Config::Context->ClearDepthStencilView(Config::DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	Config::Context->OMSetRenderTargets(1, &Config::BackBufferRTV, Config::DepthStencilView);
	//Config::Context->RSSetState(0);
}

void Renderer::RenderSkybox()
{
		ID3D11Buffer* vb = cube->GetVertexBuffer();
		ID3D11Buffer* ib = cube->GetIndexBuffer();

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Config::Context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		Config::Context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		shaders.skyVS->SetMatrix4x4("view", camera->GetViewMatrix());
		shaders.skyVS->SetMatrix4x4("projection", camera->GetProjMatrix());
		shaders.skyVS->CopyAllBufferData();
		shaders.skyVS->SetShader();

		shaders.skyPS->SetShaderResourceView("Sky", skyboxComponents.skySRV);
		shaders.skyPS->SetSamplerState("BasicSampler", Config::Sampler);
		shaders.skyPS->SetShader();

		Config::Context->RSSetState(skyboxComponents.skyRasterizer);
		Config::Context->OMSetDepthStencilState(skyboxComponents.skyDepthStencilState, 0);

		Config::Context->DrawIndexed(cube->GetIndexCount(), 0, 0);

		Config::Context->RSSetState(0);
		Config::Context->OMSetDepthStencilState(0, 0);
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

void Renderer::SendSSAOKernelToShader(SimplePixelShader* pixelShader)
{
	pixelShader->SetData(
		"kernel",
		&Config::SSAOKernel[0],
		sizeof(XMFLOAT4) * MAX_KERNEL_SAMPLES
	);
	pixelShader->SetData(
		"sampleCount",
		&Config::SSAOSampleCount,
		sizeof(Config::SSAOSampleCount)
	);
	pixelShader->SetData(
		"kernelRadius",
		&Config::SSAOKernelRadius,
		sizeof(Config::SSAOKernelRadius)
	);
}

void Renderer::AddRenderObject(Entity* e, Mesh* mesh, Material* mat)
{
	RenderObject r;

	float transparency = mat->GetMaterialData().Transparency;
	RenderObject* objects = (transparency != 1.0f) ? transparentObjects : renderObjects;
	int& count = (transparency != 1.0f) ? transparentObjectCount : renderObjectCount;
	int& maxObjects = (transparency != 1.0f) ? maxTransparentObjects : maxRenderObjects;

	if (!mesh->HasChildren()) {
		r = { e, mesh, mat };
		objects[count] = r;
		if (!renderObjectsMap.count(e)) renderObjectsMap.insert({ e, vector<RenderObject*>() });
		renderObjectsMap[e].push_back(&objects[count]);
		count++;
		if (count >= maxObjects) {
			RenderObject* old = objects;
			objects = new RenderObject[(size_t)maxObjects * 2];
			memcpy(objects, old, sizeof(RenderObject) * maxObjects);
			maxObjects *= 2;
			delete[] old;
		}
	}
	else {
		Mesh** children = mesh->GetChildren();
		for (size_t i = 0; i < mesh->GetChildCount(); i++)
		{
			r = { e, children[i], e->GetMaterial(e->GetMeshMaterialName(i)) };
			objects[count] = r;
			if (!renderObjectsMap.count(e)) renderObjectsMap.insert({ e, vector<RenderObject*>() });
			renderObjectsMap[e].push_back(&objects[count]);
			count++;
			if (count >= maxObjects) {
				RenderObject* old = objects;
				objects = new RenderObject[(size_t)maxObjects * 2];
				memcpy(objects, old, sizeof(RenderObject) * maxObjects);
				maxObjects *= 2;
				delete[] old;
			}
		}
	}
}

void Renderer::SetRenderObjectCallback(Entity* e, RendererCallback* callback)
{
	int num = renderObjectsMap[e].size();
	for (int i = 0; i < num; i++)
	{
		renderObjectsMap[e][i]->callback = callback;
	}
}
