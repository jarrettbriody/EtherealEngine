#include "pch.h"
#include "Renderer.h"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer()
{
	EELightHandler = LightHandler::GetInstance();

	maxRenderObjects = 512;
	maxTransparentObjects = maxRenderObjects;
	renderObjects = new RenderObject[maxRenderObjects];
	transparentObjects = new RenderObject[maxTransparentObjects];
}

Renderer::~Renderer()
{
	for (size_t i = 0; i < shadowComponents.cascadeCount; i++)
	{
		if (shadowComponents.shadowCascades[i].DSV)shadowComponents.shadowCascades[i].DSV->Release();
		if (shadowComponents.shadowCascades[i].SRV)shadowComponents.shadowCascades[i].SRV->Release();
	}
	if(shadowComponents.Rasterizer) shadowComponents.Rasterizer->Release();
	if(shadowComponents.Sampler) shadowComponents.Sampler->Release();

	if(depthStencilComponents.depthStencilDSV) depthStencilComponents.depthStencilDSV->Release();
	if(depthStencilComponents.depthStencilSRV) depthStencilComponents.depthStencilSRV->Release();
	if(depthStencilComponents.depthStencilRTV) depthStencilComponents.depthStencilRTV->Release();
	if(depthStencilComponents.depthStencilRasterizer) depthStencilComponents.depthStencilRasterizer->Release();
	if(depthStencilComponents.depthStencilSampler) depthStencilComponents.depthStencilSampler->Release();

	if (depthStencilComponents.entityInfoSRV) depthStencilComponents.entityInfoSRV->Release();
	if (depthStencilComponents.entityInfoRTV) depthStencilComponents.entityInfoRTV->Release();

	if (depthStencilComponents.normalSRV) depthStencilComponents.normalSRV->Release();
	if (depthStencilComponents.normalRTV) depthStencilComponents.normalRTV->Release();

	if(depthStencilComponents.depthStencilState) depthStencilComponents.depthStencilState->Release();
	if(depthStencilComponents.decalBlendState) depthStencilComponents.decalBlendState->Release();

	if(skyboxComponents.DepthStencilState) skyboxComponents.DepthStencilState->Release();
	if(skyboxComponents.Rasterizer) skyboxComponents.Rasterizer->Release();

	if (postProcessComponents.RTV) postProcessComponents.RTV->Release();
	if (postProcessComponents.SRV) postProcessComponents.SRV->Release();

	for (size_t i = 0; i < MAX_POST_PROCESS_EFFECTS; i++)
	{
		if (postProcessComponents.tempRTV[i]) postProcessComponents.tempRTV[i]->Release();
		if (postProcessComponents.tempSRV[i]) postProcessComponents.tempSRV[i]->Release();
	}

	blendState->Release();

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
	LightContainer* sun = EELightHandler->GetLight("Sun");
	XMFLOAT3 dir = sun->light.Direction;
	XMFLOAT3 pos = camera->GetTransform().GetPosition();
	pos.y = 0.0f;
	XMStoreFloat3(&pos, XMVectorAdd(XMLoadFloat3(&sun->light.Position), XMLoadFloat3(&pos)));
	XMMATRIX shadowView = XMMatrixTranspose(XMMatrixLookToLH(
		XMVectorSet(pos.x,pos.y,pos.z, 0),
		XMVectorSet(dir.x, dir.y, dir.z, 0),
		XMVectorSet(0, 1, 0, 0)));
	XMStoreFloat4x4(&shadowComponents.view, shadowView);
	shadowComponents.sunPos = pos;

	XMMATRIX shadowProj = XMMatrixTranspose(XMMatrixOrthographicLH(
		shadowComponents.shadowCascades[cascadeIndex].width,
		shadowComponents.shadowCascades[cascadeIndex].height,
		shadowComponents.shadowCascades[cascadeIndex].nearPlane,
		shadowComponents.shadowCascades[cascadeIndex].farPlane));
	XMStoreFloat4x4(&shadowComponents.shadowCascades[cascadeIndex].proj, shadowProj);

	XMStoreFloat4x4(&shadowComponents.shadowCascades[cascadeIndex].viewProj, XMMatrixTranspose(XMMatrixMultiply(shadowView, shadowProj)));
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

void Renderer::SetMeshes(Mesh* cube, Mesh* invCube, Mesh* sphere, Mesh* cone)
{
	this->cube = cube;
	this->invCube = invCube;
	this->sphere = sphere;
	this->cone = cone;
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
	if (depthStencilComponents.normalSRV) depthStencilComponents.normalSRV->Release();
	if (depthStencilComponents.normalRTV) depthStencilComponents.normalRTV->Release();

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

	//DXGI_FORMAT_R16G16B16A16_UNORM
	// Set up the texture itself
	D3D11_TEXTURE2D_DESC normTexDesc = {};
	normTexDesc.ArraySize = 1;
	normTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	normTexDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	normTexDesc.MipLevels = 1;
	normTexDesc.Height = Config::ViewPortHeight;
	normTexDesc.Width = Config::ViewPortWidth;
	normTexDesc.SampleDesc.Count = 1;

	// Actually create the texture
	ID3D11Texture2D* normTexture;
	Config::Device->CreateTexture2D(&normTexDesc, 0, &normTexture);

	// Create the shader resource view for this texture
	D3D11_SHADER_RESOURCE_VIEW_DESC normSRVDesc = {};
	normSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	normSRVDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	normSRVDesc.Texture2D.MipLevels = 1;
	normSRVDesc.Texture2D.MostDetailedMip = 0;
	Config::Device->CreateShaderResourceView(normTexture, &normSRVDesc, &depthStencilComponents.normalSRV);

	// Make a render target view desc and RTV
	D3D11_RENDER_TARGET_VIEW_DESC normRTVDesc = {};
	normRTVDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	normRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	normRTVDesc.Texture2D.MipSlice = 0;
	Config::Device->CreateRenderTargetView(normTexture, &normRTVDesc, &depthStencilComponents.normalRTV);

	// Clean up extra texture ref
	normTexture->Release();
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
	hbaoPlusComponents.Input.DepthData.MetersToViewSpaceUnits = 3.0f;

	//(3.) SET AO PARAMETERS

	hbaoPlusComponents.Params.Radius = Config::HBAORadius; // 2
	hbaoPlusComponents.Params.Bias = 0.1f;
	hbaoPlusComponents.Params.PowerExponent = Config::HBAOPowerExponent; //2
	hbaoPlusComponents.Params.Blur.Enable = Config::HBAOBlurEnabled;
	hbaoPlusComponents.Params.Blur.Radius = Config::HBAOBlurRadius; //2
	hbaoPlusComponents.Params.Blur.Sharpness = Config::HBAOBlurSharpness; //4
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
		shadowDesc.Width = shadowComponents.shadowCascades[i].resolution;
		shadowDesc.Height = shadowComponents.shadowCascades[i].resolution;
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
		Config::Device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &shadowComponents.shadowCascades[i].DSV);

		// Create the SRV for the shadow map
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		Config::Device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowComponents.shadowCascades[i].SRV);

		// Release the texture reference since we don't need it
		shadowTexture->Release();

		CalcShadowMatrices(i);
	}

	// Create the special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	//shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_ANISOTROPIC;
	shadowSampDesc.MaxAnisotropy = 16;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	Config::Device->CreateSamplerState(&shadowSampDesc, &shadowComponents.Sampler);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	Config::Device->CreateRasterizerState(&shadowRastDesc, &shadowComponents.Rasterizer);
}

void Renderer::InitSkybox()
{
	D3D11_RASTERIZER_DESC skyRD = {};
	skyRD.CullMode = D3D11_CULL_FRONT;
	skyRD.FillMode = D3D11_FILL_SOLID;
	skyRD.DepthClipEnable = true;
	Config::Device->CreateRasterizerState(&skyRD, &skyboxComponents.Rasterizer);

	D3D11_DEPTH_STENCIL_DESC skyDS = {};
	skyDS.DepthEnable = true;
	skyDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	skyDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	Config::Device->CreateDepthStencilState(&skyDS, &skyboxComponents.DepthStencilState);
}

void Renderer::InitPostProcessRTV()
{
	if (postProcessComponents.RTV) postProcessComponents.RTV->Release();
	if (postProcessComponents.SRV) postProcessComponents.SRV->Release();
	for (size_t i = 0; i < MAX_POST_PROCESS_EFFECTS; i++)
	{
		if (postProcessComponents.tempRTV[i]) postProcessComponents.tempRTV[i]->Release();
		if (postProcessComponents.tempSRV[i]) postProcessComponents.tempSRV[i]->Release();
	}

	for (int i = -1; i < MAX_POST_PROCESS_EFFECTS; i++)
	{
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = Config::ViewPortWidth;
		textureDesc.Height = Config::ViewPortHeight;
		textureDesc.ArraySize = 1;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.MipLevels = 1;
		textureDesc.MiscFlags = 0;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;

		ID3D11Texture2D* ppTexture;
		Config::Device->CreateTexture2D(&textureDesc, 0, &ppTexture);

		// Create the Render Target View
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		Config::Device->CreateRenderTargetView(ppTexture, &rtvDesc, (i == -1) ? &postProcessComponents.RTV : &postProcessComponents.tempRTV[i]);

		// Create the Shader Resource View
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		Config::Device->CreateShaderResourceView(ppTexture, &srvDesc, (i == -1) ? &postProcessComponents.SRV : &postProcessComponents.tempSRV[i]);

		// We don't need the texture reference itself no mo'
		ppTexture->Release();
	}
}

void Renderer::SetSkybox(ID3D11ShaderResourceView* srv)
{
	skyboxComponents.SRV = srv;
}

void Renderer::SetShadowCascadeInfo(unsigned int cascadeIndex, unsigned int resolution, float nearPlane, float farPlane, float width, float height)
{
	unsigned int exponent = (unsigned int)(log2((double)resolution) + 0.5);
	shadowComponents.shadowCascades[cascadeIndex].resolution = (unsigned int)(pow(2u, exponent) + 0.5);
	shadowComponents.shadowCascades[cascadeIndex].nearPlane = nearPlane;
	shadowComponents.shadowCascades[cascadeIndex].farPlane = farPlane;
	shadowComponents.shadowCascades[cascadeIndex].width = width;
	shadowComponents.shadowCascades[cascadeIndex].height = height;
	//shadowComponents.shadowCascades[cascadeIndex].maxRange = maxRange;
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
	Config::Context->ClearRenderTargetView(postProcessComponents.RTV, color);
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

	XMFLOAT3 camPos = camera->GetTransform().GetPosition();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	if(postProcessComponents.enabled) Config::Context->OMSetRenderTargets(1, &postProcessComponents.RTV, Config::DepthStencilView);

	ToggleBlendState(false);

	lightsSentToShader.clear();

	for (int i = renderObjectCount - 1; i >= 0; i--)
	{
		RenderObject renderObject = renderObjects[i];
		Entity* e = renderObject.entity;
		Mesh* mesh = renderObject.mesh;
		Material* mat = renderObject.material;
		RendererCallback* callback = renderObject.callback;

		if (e->isEmptyObj) continue;
		if (!e->renderObject) continue;
		e->ToggleShadows(Config::ShadowsEnabled);
		if (Config::ShadowsEnabled) {
			ShadowData d;
			for (size_t i = 0; i < MAX_SHADOW_CASCADES; i++)
			{
				d.shadowProjectionMatrix[i] = shadowComponents.shadowCascades[i].proj;
				d.shadowSRV[i] = shadowComponents.shadowCascades[i].SRV;
				d.nears[i] = shadowComponents.shadowCascades[i].nearPlane;
				d.fars[i] = shadowComponents.shadowCascades[i].farPlane;
				//d.range[i] = shadowComponents.shadowCascades[i].maxRange;
				d.widthHeight[i] = XMFLOAT2(shadowComponents.shadowCascades[i].width, shadowComponents.shadowCascades[i].height);
			}
			d.sunPos = shadowComponents.sunPos;
			d.shadowViewMatrix = shadowComponents.view;
			d.shadowSampler = shadowComponents.Sampler;
			d.cascadeCount = shadowComponents.cascadeCount;
			e->SetShadowData(d);
		}

		if (Config::SSAOEnabled) {
			DepthStencilData d;
			d.depthStencilSRV = depthStencilComponents.depthStencilSRV;
			d.depthStencilSampler = depthStencilComponents.depthStencilSampler;
			e->SetDepthStencilData(d);
		}

		SimplePixelShader* ps = mat->GetPixelShader();
		if (!lightsSentToShader.count(ps)) {
			ps->SetData(
				"lights",
				&EELightHandler->DrawList,
				sizeof(EELightHandler->DrawList)
			);
			ps->SetData(
				"lightCount",
				&EELightHandler->DrawCount,
				sizeof(EELightHandler->DrawCount)
			);
			ps->SetFloat3("cameraPosition", camPos);
			lightsSentToShader.insert({ ps, true });
		}

		ID3D11Buffer* vbo = mesh->GetVertexBuffer();
		ID3D11Buffer* ind = mesh->GetIndexBuffer();
		Config::Context->IASetVertexBuffers(0, 1, &vbo, &stride, &offset);
		Config::Context->IASetIndexBuffer(ind, DXGI_FORMAT_R32_UINT, 0);

		if (callback != nullptr) {
			if (callback->active && callback->vShader) {
				callback->PreVertexShaderCallback();
			}
			if (callback->active && callback->pShader) {
				callback->PrePixelShaderCallback();
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
}

void Renderer::RenderDecals()
{
	XMFLOAT3 camPos = camera->GetTransform().GetPosition();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
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

		shaders.decalVS->SetFloat3("cameraPos", camPos);

		//shaders.decalPS->SetMatrix4x4("shadowViewProj", shadowComponents.shadowViewProj);
		shaders.decalPS->SetMatrix4x4("shadowView", shadowComponents.view);

		shaders.decalPS->SetFloat3("sunPos", shadowComponents.sunPos);
		shaders.decalPS->SetShaderResourceView("DepthBuffer", depthStencilComponents.depthStencilSRV);
		shaders.decalPS->SetShaderResourceView("EntityInfoBuffer", depthStencilComponents.entityInfoSRV);

		for (size_t i = 0; i < MAX_SHADOW_CASCADES; i++)
		{
			string str = to_string(i);
			shaders.decalPS->SetShaderResourceView("ShadowMap" + str, shadowComponents.shadowCascades[i].SRV);
			shaders.decalPS->SetMatrix4x4("shadowProj" + str, shadowComponents.shadowCascades[i].proj);
			shaders.decalPS->SetFloat2("cascadeRange" + str, XMFLOAT2(shadowComponents.shadowCascades[i].width, shadowComponents.shadowCascades[i].height));
		}

		shaders.decalPS->SetSamplerState("ShadowSampler", shadowComponents.Sampler);
		shaders.decalPS->SetFloat3("cameraPos", camPos);
		shaders.decalPS->SetFloat("brightness", Config::SceneBrightness);

		shaders.decalPS->SetData(
			"lights",
			&EELightHandler->DrawList,
			sizeof(EELightHandler->DrawList)
		);
		shaders.decalPS->SetData(
			"lightCount",
			&EELightHandler->DrawCount,
			sizeof(EELightHandler->DrawCount)
		);

		shaders.decalPS->SetData("decalLayerMask", &Config::EntityLayers["decal"], sizeof(unsigned int));

		XMFLOAT4X4 world;
		XMFLOAT4X4 invWorld;
		XMMATRIX ownerWorld;
		XMMATRIX localWorld;
		XMMATRIX cWorld;
		for (size_t i = 0; i < DecalHandler::decalsVec.size(); i++)
		{
			DecalBucket* db = DecalHandler::decalsVec[i];
			ownerWorld = XMMatrixTranspose(XMLoadFloat4x4(&db->owner->GetTransform().GetWorldMatrix()));
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
}

void Renderer::RenderHBAOPlus()
{
	if (Config::HBAOPlusEnabled) {
		//(4.) RENDER AO
		if (postProcessComponents.enabled) {
			hbaoPlusComponents.status = hbaoPlusComponents.pAOContext->RenderAO(Config::Context, &hbaoPlusComponents.Input, &hbaoPlusComponents.Params, postProcessComponents.RTV);
		}
		else {
			hbaoPlusComponents.status = hbaoPlusComponents.pAOContext->RenderAO(Config::Context, &hbaoPlusComponents.Input, &hbaoPlusComponents.Params, Config::BackBufferRTV);
		}
		assert(hbaoPlusComponents.status == GFSDK_SSAO_OK);
	}
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
	if (!entities || !shaders.depthStencilVS || !Config::ShadowsEnabled || renderObjectCount == 0) {
		return;
	}

	D3D11_VIEWPORT vp = {};

	for (size_t j = 0; j < shadowComponents.cascadeCount; j++)
	{
		// Initial setup - No RTV necessary - Clear shadow map
		Config::Context->OMSetRenderTargets(0, 0, shadowComponents.shadowCascades[j].DSV);
		Config::Context->ClearDepthStencilView(shadowComponents.shadowCascades[j].DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
		Config::Context->RSSetState(shadowComponents.Rasterizer);

		// SET A VIEWPORT!!!
		vp = {};
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)shadowComponents.shadowCascades[j].resolution;
		vp.Height = (float)shadowComponents.shadowCascades[j].resolution;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		Config::Context->RSSetViewports(1, &vp);

		CalcShadowMatrices();

		// Set up the shaders
		shaders.depthStencilVS->SetShader();
		shaders.depthStencilVS->SetMatrix4x4("view", shadowComponents.view);
		shaders.depthStencilVS->SetMatrix4x4("projection", shadowComponents.shadowCascades[j].proj);
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

			if (e->destroyed) {
				if (i != renderObjectCount - 1) {
					renderObjects[i] = renderObjects[renderObjectCount - 1];
				}
				//renderObjectsMap.erase(e->GetName());
				renderObjectCount--;
				continue;
			}

			if (e->isEmptyObj) continue;
			if (!e->renderObject) continue;

			//if (mat->GetVertexShader()->GetShaderType() == ShaderType::MODIFY_VERTS) {}

			ID3D11Buffer* vb = mesh->GetVertexBuffer();
			ID3D11Buffer* ib = mesh->GetIndexBuffer();

			// Set buffers in the input assembler
			Config::Context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
			Config::Context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

			if (callback != nullptr) {
				if (callback->active && callback->prepassVShader) {
					callback->prepassVShader->SetShader();
					callback->prepassVShader->SetMatrix4x4("world", e->GetTransform().GetWorldMatrix());
					callback->prepassVShader->SetMatrix4x4("view", shadowComponents.view);
					callback->prepassVShader->SetMatrix4x4("projection", shadowComponents.shadowCascades[j].proj);
					callback->PrePrepassVertexShaderCallback();
					callback->prepassVShader->CopyAllBufferData();
				}
				else {
					shaders.depthStencilVS->SetMatrix4x4("world", e->GetTransform().GetWorldMatrix());
					shaders.depthStencilVS->CopyAllBufferData();
				}
			}
			else {
				shaders.depthStencilVS->SetMatrix4x4("world", e->GetTransform().GetWorldMatrix());
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
		//depthStencilComponents.normalRTV,
	};
	Config::Context->OMSetRenderTargets(2, targets, Config::DepthStencilView);

	//Config::Context->OMSetRenderTargets(1, &depthStencilComponents.depthStencilRTV, Config::DepthStencilView);
	//Config::Context->ClearDepthStencilView(depthStencilComponents.depthStencilDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Config::Context->ClearRenderTargetView(depthStencilComponents.depthStencilRTV, color);
	Config::Context->ClearRenderTargetView(depthStencilComponents.entityInfoRTV, color);
	Config::Context->ClearRenderTargetView(depthStencilComponents.normalRTV, color);

	//Config::Context->RSSetState(depthStencilComponents.depthStencilRasterizer);

	XMFLOAT4X4 camWorld;
	XMFLOAT4X4 camView = camera->GetViewMatrix();
	XMFLOAT4X4 camProj = camera->GetProjMatrix();
	XMFLOAT3 camPos = camera->GetTransform().GetPosition();

	// Set up the shaders
	shaders.depthStencilVS->SetShader();
	shaders.depthStencilPS->SetShader();
	shaders.depthStencilVS->SetMatrix4x4("view", camView);
	shaders.depthStencilVS->SetMatrix4x4("projection", camProj);
	shaders.depthStencilPS->SetFloat3("cameraPosition", camPos);

	//Config::Context->PSSetShader(0, 0, 0); // Turns OFF the pixel shader

	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	unsigned int entityLayerMask;
	//unsigned int callbackLayerMask = Config::EntityLayers["decal"];


	for (int i = renderObjectCount - 1; i >= 0; i--)
	{
		shaders.depthStencilVS->SetShader();
		shaders.depthStencilPS->SetShader();

		RenderObject renderObject = renderObjects[i];
		Entity* e = renderObject.entity;
		Mesh* mesh = renderObject.mesh;
		Material* mat = renderObject.material;
		RendererCallback* callback = renderObject.callback;

		if (e->isEmptyObj || !renderObject.material->GetMaterialData().hbaoPlusEnabled || !renderObject.entity->hbaoPlusEnabled) continue;

		if (!e->renderObject) continue;

		//if (mat->GetVertexShader()->GetShaderType() == ShaderType::MODIFY_VERTS) {}

		ID3D11Buffer* vb = mesh->GetVertexBuffer();
		ID3D11Buffer* ib = mesh->GetIndexBuffer();

		// Set buffers in the input assembler
		Config::Context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		Config::Context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		if (callback != nullptr) {
			if (callback->active && callback->prepassVShader) {
				callback->prepassVShader->SetShader();
				callback->prepassVShader->SetMatrix4x4("world", e->GetTransform().GetWorldMatrix());
				callback->prepassVShader->SetMatrix4x4("view", camView);
				callback->prepassVShader->SetMatrix4x4("projection", camProj);
				callback->PrePrepassVertexShaderCallback();
				callback->prepassVShader->CopyAllBufferData();
			}
			else {
				shaders.depthStencilVS->SetMatrix4x4("world", e->GetTransform().GetWorldMatrix());
				shaders.depthStencilVS->CopyAllBufferData();
			}
		}
		else {
			shaders.depthStencilVS->SetMatrix4x4("world", e->GetTransform().GetWorldMatrix());
			shaders.depthStencilVS->CopyAllBufferData();
		}

		entityLayerMask = e->GetLayerMask();
		if (callback != nullptr) {
			if (callback->active && callback->prepassPShader) {
				callback->prepassPShader->SetShader();
				callback->prepassPShader->SetData("entityLayerMask", &entityLayerMask, sizeof(unsigned int));
				callback->prepassPShader->SetFloat3("cameraPosition", camPos);
				callback->PrePrepassPixelShaderCallback();
				callback->prepassPShader->CopyAllBufferData();
			}
			else {
				shaders.depthStencilPS->SetData("entityLayerMask", &entityLayerMask, sizeof(unsigned int));
				shaders.depthStencilPS->CopyAllBufferData();
			}
		}
		else {
			shaders.depthStencilPS->SetData("entityLayerMask", &entityLayerMask, sizeof(unsigned int));
			shaders.depthStencilPS->CopyAllBufferData();
		}

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

		shaders.skyPS->SetShaderResourceView("Sky", skyboxComponents.SRV);
		shaders.skyPS->SetSamplerState("BasicSampler", Config::Sampler);
		shaders.skyPS->SetShader();

		Config::Context->RSSetState(skyboxComponents.Rasterizer);
		Config::Context->OMSetDepthStencilState(skyboxComponents.DepthStencilState, 0);

		Config::Context->DrawIndexed(cube->GetIndexCount(), 0, 0);

		Config::Context->RSSetState(0);
		Config::Context->OMSetDepthStencilState(0, 0);
}

void Renderer::RenderTransparents()
{
	ToggleBlendState(true);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (int i = transparentObjectCount - 1; i >= 0; i--)
	{
		RenderObject renderObject = transparentObjects[i];
		Entity* e = renderObject.entity;
		Mesh* mesh = renderObject.mesh;
		Material* mat = renderObject.material;
		RendererCallback* callback = renderObject.callback;

		if (e->destroyed || e->isEmptyObj) {
			if (i != transparentObjectCount - 1) {
				transparentObjects[i] = transparentObjects[transparentObjectCount - 1];
			}
			//renderObjectsMap.erase(e->GetName());
			transparentObjectCount--;
			continue;
		}

		if (e->isEmptyObj) continue;
		if (!e->renderObject) continue;

		e->ToggleShadows(Config::ShadowsEnabled);
		if (Config::ShadowsEnabled) {
			ShadowData d;
			for (size_t i = 0; i < MAX_SHADOW_CASCADES; i++)
			{
				d.shadowProjectionMatrix[i] = shadowComponents.shadowCascades[i].proj;
				d.shadowSRV[i] = shadowComponents.shadowCascades[i].SRV;
				d.nears[i] = shadowComponents.shadowCascades[i].nearPlane;
				d.fars[i] = shadowComponents.shadowCascades[i].farPlane;
				//d.range[i] = shadowComponents.shadowCascades[i].maxRange;
				d.widthHeight[i] = XMFLOAT2(shadowComponents.shadowCascades[i].width, shadowComponents.shadowCascades[i].height);
			}
			d.sunPos = shadowComponents.sunPos;
			d.shadowViewMatrix = shadowComponents.view;
			d.shadowSampler = shadowComponents.Sampler;
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
			if (callback->active && callback->pShader) {
				callback->PrePixelShaderCallback();
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
	ToggleBlendState(false);
}

void Renderer::RenderPostProcess()
{
	if (!postProcessComponents.enabled) return;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	unsigned int finalPostProcess = 0;
	for (size_t i = 0; i < MAX_POST_PROCESS_EFFECTS; i++)
	{
		RendererCallback* callback = postProcessComponents.callbacks[i];
		if (callback == nullptr) continue;
		if (!callback->active) continue;
		finalPostProcess = i;
	}

	unsigned int lastPostProcess = 0;
	for (size_t i = 0; i < MAX_POST_PROCESS_EFFECTS; i++)
	{
		RendererCallback* callback = postProcessComponents.callbacks[i];
		if (callback == nullptr) continue;
		if (!callback->active) continue;

		if(i == finalPostProcess)
			Config::Context->OMSetRenderTargets(1, &Config::BackBufferRTV, 0);
		else
			Config::Context->OMSetRenderTargets(1, &postProcessComponents.tempRTV[i], 0);

		callback->vShader->SetShader();
		callback->PreVertexShaderCallback();
		callback->vShader->CopyAllBufferData();
		
		callback->pShader->SetShader();
		callback->PrePixelShaderCallback();
		callback->pShader->SetShaderResourceView("Pixels", (i==0) ? postProcessComponents.SRV : postProcessComponents.tempSRV[lastPostProcess]);
		callback->pShader->SetSamplerState("Sampler", Config::Sampler);
		callback->pShader->SetFloat("pixelWidth", 1.0f / Config::ViewPortWidth);
		callback->pShader->SetFloat("pixelHeight", 1.0f / Config::ViewPortHeight);
		callback->pShader->CopyAllBufferData();

		// Deactivate vertex and index buffers
		ID3D11Buffer* nothing = 0;
		Config::Context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
		Config::Context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

		// Draw a set number of vertices
		Config::Context->Draw(3, 0);

		// Unbind all pixel shader SRVs
		ID3D11ShaderResourceView* nullSRVs[16] = {};
		Config::Context->PSSetShaderResources(0, 16, nullSRVs);

		lastPostProcess = i;
	}
}

void Renderer::RenderUI()
{
	if (!uiComponents.enabled) return;

	for (size_t i = 0; i < MAX_UI_CALLBACKS; i++)
	{
		if(uiComponents.UICallbacks[i] != nullptr)
			uiComponents.UICallbacks[i]->Call();
	}
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
	RenderObject** objects = (transparency != 1.0f) ? &transparentObjects : &renderObjects;
	int& count = (transparency != 1.0f) ? transparentObjectCount : renderObjectCount;
	int& maxObjects = (transparency != 1.0f) ? maxTransparentObjects : maxRenderObjects;

	if (!mesh->HasChildren()) {
		r = { e, mesh, mat };
		(*objects)[count] = r;
		//if (!renderObjectsMap.count(e->GetName())) renderObjectsMap.insert({ e->GetName(), vector<RenderObject*>() });
		//renderObjectsMap[e->GetName()].push_back(&(*objects)[count]);
		count++;
		if (count >= maxObjects) {
			RenderObject* oldObjects = *objects;
			RenderObject* newObjects = new RenderObject[(size_t)maxObjects * 2];
			memcpy(newObjects, *objects, sizeof(RenderObject) * maxObjects);
			maxObjects *= 2;
			*objects = newObjects;
			delete[] oldObjects;
		}
	}
	else {
		Mesh** children = mesh->GetChildren();
		for (size_t i = 0; i < mesh->GetChildCount(); i++)
		{
			r = { e, children[i], e->GetMaterial(e->GetMeshMaterialName(i)) };
			(*objects)[count] = r;
			//if (!renderObjectsMap.count(e->GetName())) renderObjectsMap.insert({ e->GetName(), vector<RenderObject*>() });
			//renderObjectsMap[e->GetName()].push_back(&(*objects)[count]);
			count++;
			if (count >= maxObjects) {
				RenderObject* oldObjects = *objects;
				RenderObject* newObjects = new RenderObject[(size_t)maxObjects * 2];
				memcpy(newObjects, *objects, sizeof(RenderObject) * maxObjects);
				maxObjects *= 2;
				*objects = newObjects;
				delete[] oldObjects;
			}
		}
	}
}

void Renderer::ClearRenderer()
{
	renderObjectCount = 0;
	transparentObjectCount = 0;
	for (size_t i = 0; i < MAX_POST_PROCESS_EFFECTS; i++)
	{
		postProcessComponents.callbacks[i] = nullptr;
	}
	for (size_t i = 0; i < MAX_UI_CALLBACKS; i++)
	{
		uiComponents.UICallbacks[i] = nullptr;
	}
}

void Renderer::SetRenderObjectCallback(Entity* e, RendererCallback* callback)
{
	/*int num = renderObjectsMap[e->GetName()].size();
	for (int i = 0; i < num; i++)
	{
		renderObjectsMap[e->GetName()][i]->callback = callback;
	}*/
	for (size_t i = 0; i < renderObjectCount; i++)
	{
		if (renderObjects[i].entity == e) {
			renderObjects[i].callback = callback;
		}
	}
	for (size_t i = 0; i < transparentObjectCount; i++)
	{
		if (transparentObjects[i].entity == e) {
			transparentObjects[i].callback = callback;
		}
	}
}

void Renderer::SetPostProcess(bool toggle, RendererCallback* callback, unsigned int index)
{
	postProcessComponents.enabled = toggle;
	if (!toggle || (toggle && callback == nullptr) || index >= MAX_POST_PROCESS_EFFECTS) return;
	postProcessComponents.callbacks[index] = callback;
}

void Renderer::SetRenderUICallback(bool toggle, Utility::Callback* cb, unsigned int index)
{
	uiComponents.enabled = true;
	if (!toggle || (toggle && cb == nullptr) || index >= MAX_UI_CALLBACKS) return;
	uiComponents.UICallbacks[index] = cb;
}
