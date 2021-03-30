#include "pch.h"
#include "Renderer.h"

Renderer* Renderer::instance = nullptr;

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
	shadowComponents.shadowDSV->Release();
	shadowComponents.shadowSRV->Release();
	shadowComponents.shadowRasterizer->Release();
	shadowComponents.shadowSampler->Release();

	//depthStencilComponents.depthStencilDSV->Release();
	depthStencilComponents.depthStencilSRV->Release();
	depthStencilComponents.depthStencilRTV->Release();
	//depthStencilComponents.depthStencilRasterizer->Release();
	//depthStencilComponents.depthStencilSampler->Release();
	depthStencilComponents.depthStencilState->Release();
	depthStencilComponents.decalBlendState->Release();

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

	if(hbaoPlusComponents.pAOContext)
		hbaoPlusComponents.pAOContext->Release();
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

void Renderer::SetRendererShaders(RendererShaders rShaders)
{
	shaders = rShaders;
}

void Renderer::SetDecals(Mesh* cube, ID3D11ShaderResourceView* decals[8])
{
	this->cube = cube;
	for (size_t i = 0; i < 8; i++)
	{
		this->decals[i] = decals[i];
	}
}

void Renderer::InitDepthStencil()
{
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
	/*
	// Create the actual texture that will be the depth stencil map
	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = Config::ViewPortWidth;
	depthStencilDesc.Height = Config::ViewPortHeight;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.MiscFlags = 0;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* depthStencilTexture;
	Config::Device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilTexture);

	// Create the depth/stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDSDesc = {};
	depthStencilDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilDSDesc.Texture2D.MipSlice = 0;
	Config::Device->CreateDepthStencilView(depthStencilTexture, &depthStencilDSDesc, &depthStencilComponents.depthStencilDSV);

	// Create the SRV for the depth stencil map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Config::Device->CreateShaderResourceView(depthStencilTexture, &srvDesc, &depthStencilComponents.depthStencilSRV);

	// Release the texture reference since we don't need it
	depthStencilTexture->Release();

	// Create the special "comparison" sampler state for depth stencils
	D3D11_SAMPLER_DESC depthStencilSampDesc = {};
	depthStencilSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	depthStencilSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	depthStencilSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	depthStencilSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	depthStencilSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	depthStencilSampDesc.BorderColor[0] = 1.0f;
	depthStencilSampDesc.BorderColor[1] = 1.0f;
	depthStencilSampDesc.BorderColor[2] = 1.0f;
	depthStencilSampDesc.BorderColor[3] = 1.0f;
	Config::Device->CreateSamplerState(&depthStencilSampDesc, &depthStencilComponents.depthStencilSampler);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC depthStencilRastDesc = {};
	depthStencilRastDesc.FillMode = D3D11_FILL_SOLID;
	depthStencilRastDesc.CullMode = D3D11_CULL_BACK;
	depthStencilRastDesc.DepthClipEnable = true;
	depthStencilRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	depthStencilRastDesc.DepthBiasClamp = 0.0f;
	depthStencilRastDesc.SlopeScaledDepthBias = 1.0f;
	Config::Device->CreateRasterizerState(&depthStencilRastDesc, &depthStencilComponents.depthStencilRasterizer);

	*/

	D3D11_DEPTH_STENCIL_DESC dsDesc;

	dsDesc.DepthEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
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
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
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

void Renderer::InitShadows()
{
	// Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowComponents.shadowMapResolution;
	shadowDesc.Height = shadowComponents.shadowMapResolution;
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
	Config::Device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &shadowComponents.shadowDSV);

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Config::Device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowComponents.shadowSRV);

	// Release the texture reference since we don't need it
	shadowTexture->Release();

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

	// Create the matrices that represent seeing the scene from
	// the light's point of view
	XMFLOAT3 dir = lights["Sun"]->Direction;
	XMMATRIX shadowView = XMMatrixTranspose(XMMatrixLookToLH(
		XMVectorSet(lights["Sun"]->Position.x, lights["Sun"]->Position.y, lights["Sun"]->Position.z, 0),
		XMVectorSet(dir.x, dir.y, dir.z, 0),
		XMVectorSet(0, 1, 0, 0)));
	XMStoreFloat4x4(&shadowComponents.shadowViewMatrix, shadowView);

	XMMATRIX shadowProj = XMMatrixTranspose(XMMatrixOrthographicLH(
		750.0f,
		750.0f,
		0.1f,
		1000.0f));
	XMStoreFloat4x4(&shadowComponents.shadowProjectionMatrix, shadowProj);

	XMStoreFloat4x4(&shadowComponents.shadowViewProj, XMMatrixTranspose(XMMatrixMultiply(shadowView, shadowProj)));
}

void Renderer::SetShadowMapResolution(unsigned int res)
{
	unsigned int exponent = (unsigned int)(log2((double)res) + 0.5);
	shadowComponents.shadowMapResolution = (unsigned int)(pow(2u, exponent) + 0.5);
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
		e->ToggleShadows(Config::ShadowsEnabled);
		if (Config::ShadowsEnabled) {
			ShadowData d;
			d.shadowViewMatrix = shadowComponents.shadowViewMatrix;
			d.shadowProjectionMatrix = shadowComponents.shadowProjectionMatrix;
			d.shadowSRV = shadowComponents.shadowSRV;
			d.shadowSampler = shadowComponents.shadowSampler;
			e->SetShadowData(d);
		}

		if (Config::SSAOEnabled) {
			DepthStencilData d;
			d.depthStencilSRV = depthStencilComponents.depthStencilSRV;
			d.depthStencilSampler = depthStencilComponents.depthStencilSampler;
			e->SetDepthStencilData(d);
		}
		
		/*
		if (*e->layer == "decal") {
			SimplePixelShader* pixelShader = mat->GetPixelShader();
			if (DecalHandler::decalsMap.count(e->GetName())) {
				DecalBucket& bucket = DecalHandler::decalsMap[e->GetName()];
				pixelShader->SetData(
					"decals",
					&bucket.decals,
					sizeof(Decal) * MAX_DECALS_PER_ENTITY
				);
				pixelShader->SetData(
					"decalCount",
					&bucket.count,
					sizeof(bucket.count)
				);
				pixelShader->SetMatrix4x4("worldMatrix", e->GetWorldMatrix());
				pixelShader->SetShaderResourceView("Decals", decals[0]);
			}
		}
		*/

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

	if (Config::DecalsEnabled) {
		//ID3D11BlendState* originalBlend;
		//Config::Context->OMGetBlendState(&originalBlend);
		Config::Context->OMSetDepthStencilState(depthStencilComponents.depthStencilState, 0);
		//Config::Context->OMSetBlendState(depthStencilComponents.decalBlendState, 0, 0xFFFFFFFF);
		ID3D11Buffer* vbo = cube->GetVertexBuffer();
		ID3D11Buffer* ind = cube->GetIndexBuffer();
		Config::Context->IASetVertexBuffers(0, 1, &vbo, &stride, &offset);
		Config::Context->IASetIndexBuffer(ind, DXGI_FORMAT_R32_UINT, 0);

		shaders.decalVS->SetShader();
		shaders.decalPS->SetShader();

		shaders.decalVS->SetFloat3("cameraPos", camera->position);

		//shaders.decalPS->SetMatrix4x4("shadowViewProj", shadowComponents.shadowViewProj);
		shaders.decalPS->SetMatrix4x4("shadowView", shadowComponents.shadowViewMatrix);
		shaders.decalPS->SetMatrix4x4("shadowProj", shadowComponents.shadowProjectionMatrix);
		shaders.decalPS->SetShaderResourceView("DepthBuffer", depthStencilComponents.depthStencilSRV);
		shaders.decalPS->SetShaderResourceView("ShadowMap", shadowComponents.shadowSRV);
		shaders.decalPS->SetSamplerState("ShadowSampler", shadowComponents.shadowSampler);
		shaders.decalPS->SetFloat3("cameraPos", camera->position);

		for (size_t i = 0; i < DecalHandler::decalsVec.size(); i++)
		{
			DecalBucket* db = DecalHandler::decalsVec[i];
			for (size_t j = 0; j < db->count; j++)
			{
				XMFLOAT4X4 world;
				XMFLOAT4X4 invWorld;
				XMMATRIX ownerWorld = XMMatrixTranspose(XMLoadFloat4x4(&db->owner->GetWorldMatrix()));
				XMMATRIX localWorld = XMMatrixTranspose(XMLoadFloat4x4(&db->decals[j].localTransform));
				XMMATRIX cWorld = XMMatrixMultiply(localWorld, ownerWorld);
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
					cube->GetIndexCount(),						// The number of indices to use (we could draw a subset if we wanted)
					0,											// Offset to the first index we want to use
					0);											// Offset to add to each index when looking up vertices
			}
		}
		shaders.decalPS->SetShaderResourceView("ShadowMap", NULL);
		shaders.decalPS->SetShaderResourceView("DepthBuffer", NULL);
		Config::Context->OMSetDepthStencilState(NULL, 0);
		//Config::Context->OMSetBlendState(NULL, 0, 0xFFFFFFFF);
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

	// Initial setup - No RTV necessary - Clear shadow map
	Config::Context->OMSetRenderTargets(0, 0, shadowComponents.shadowDSV);
	Config::Context->ClearDepthStencilView(shadowComponents.shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	Config::Context->RSSetState(shadowComponents.shadowRasterizer);

	// SET A VIEWPORT!!!
	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)shadowComponents.shadowMapResolution;
	vp.Height = (float)shadowComponents.shadowMapResolution;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	Config::Context->RSSetViewports(1, &vp);

	// Set up the shaders
	shaders.depthStencilVS->SetShader();
	//shaders.depthStencilPS->SetShader();
	shaders.depthStencilVS->SetMatrix4x4("view", shadowComponents.shadowViewMatrix);
	shaders.depthStencilVS->SetMatrix4x4("projection", shadowComponents.shadowProjectionMatrix);
	//shaders.depthStencilPS->SetFloat3("cameraPosition", lights["Sun"]->Position);
	//shaders.depthStencilPS->CopyAllBufferData();

	Config::Context->PSSetShader(0, 0, 0); // Turns OFF the pixel shader

	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (int i = renderObjectCount - 1; i >= 0; i--)
	{
		RenderObject renderObject = renderObjects[i];
		Entity* e = renderObject.entity;
		Mesh* mesh = renderObject.mesh;

		if (e->destroyed || e->isEmptyObj) {
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


		shaders.depthStencilVS->SetMatrix4x4("world", e->GetWorldMatrix());
		shaders.depthStencilVS->CopyAllBufferData();

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

void Renderer::RenderDepthStencil()
{
	if (!entities || !shaders.depthStencilVS || (!Config::SSAOEnabled && !Config::HBAOPlusEnabled) || renderObjectCount == 0)
		return;

	// Initial setup - No RTV necessary - Clear depthStencil map
	//Config::Context->OMSetRenderTargets(0, 0, depthStencilComponents.depthStencilDSV);
	Config::Context->OMSetRenderTargets(1, &depthStencilComponents.depthStencilRTV, Config::DepthStencilView);
	//Config::Context->ClearDepthStencilView(depthStencilComponents.depthStencilDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Config::Context->ClearRenderTargetView(depthStencilComponents.depthStencilRTV, color);
	//Config::Context->RSSetState(depthStencilComponents.depthStencilRasterizer);

	// Set up the shaders
	shaders.depthStencilVS->SetShader();
	shaders.depthStencilPS->SetShader();
	shaders.depthStencilVS->SetMatrix4x4("view", camera->GetViewMatrix());
	shaders.depthStencilVS->SetMatrix4x4("projection", camera->GetProjMatrix());
	shaders.depthStencilPS->SetFloat3("cameraPosition", camera->position);
	shaders.depthStencilPS->CopyAllBufferData();

	//Config::Context->PSSetShader(0, 0, 0); // Turns OFF the pixel shader

	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	for (int i = renderObjectCount - 1; i >= 0; i--)
	{
		RenderObject renderObject = renderObjects[i];
		Entity* e = renderObject.entity;
		Mesh* mesh = renderObject.mesh;

		if (e->isEmptyObj) continue;

		ID3D11Buffer* vb = mesh->GetVertexBuffer();
		ID3D11Buffer* ib = mesh->GetIndexBuffer();

		// Set buffers in the input assembler
		Config::Context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		Config::Context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);


		shaders.depthStencilVS->SetMatrix4x4("world", e->GetWorldMatrix());
		shaders.depthStencilVS->CopyAllBufferData();

		// Finally do the actual drawing
		Config::Context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}

	// Revert to original pipeline state

	Config::Context->ClearDepthStencilView(Config::DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	Config::Context->OMSetRenderTargets(1, &Config::BackBufferRTV, Config::DepthStencilView);
	//Config::Context->RSSetState(0);
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
