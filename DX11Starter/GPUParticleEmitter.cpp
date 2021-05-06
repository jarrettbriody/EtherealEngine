#include "pch.h"
#include "GPUParticleEmitter.h"

DefaultGPUParticleShaders GPUParticleEmitter::defaultShaders;

void GPUParticleEmitter::SetDefaultShaders(DefaultGPUParticleShaders s)
{
	defaultShaders = s;
}

GPUParticleEmitter::GPUParticleEmitter() : ParticleEmitter()
{
	InitBuffers();
}

GPUParticleEmitter::GPUParticleEmitter(ParticleEmitterDescription d, bool blendingEnabled) : ParticleEmitter(d)
{
	this->blendingEnabled = blendingEnabled;
	InitBuffers();
}

GPUParticleEmitter::~GPUParticleEmitter()
{
	particlePoolUAV->Release();
	particleDrawUAV->Release();
	particleDeadUAV->Release();

	particlePoolSRV->Release();
	particleDrawSRV->Release();

	drawArgsBuffer->Release();
	drawArgsUAV->Release();
	randUAV->Release();

	indexBuffer->Release();
	additiveBlend->Release();
	depthWriteOff->Release();
}

void GPUParticleEmitter::InitBuffers()
{
	// Particle index buffer =================
	{
		// Buffer
		D3D11_BUFFER_DESC ibDesc = {};
		ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibDesc.ByteWidth = sizeof(unsigned long) * maxParticles * 6;
		ibDesc.CPUAccessFlags = 0;
		ibDesc.MiscFlags = 0;
		ibDesc.StructureByteStride = 0;
		ibDesc.Usage = D3D11_USAGE_DEFAULT;

		// Fill it with data
		unsigned long* indices = new unsigned long[maxParticles * 6];
		for (unsigned long i = 0; i < maxParticles; i++)
		{
			unsigned long indexCounter = i * 6;
			indices[indexCounter + 0] = 0 + i * 4;
			indices[indexCounter + 1] = 1 + i * 4;
			indices[indexCounter + 2] = 2 + i * 4;
			indices[indexCounter + 3] = 0 + i * 4;
			indices[indexCounter + 4] = 2 + i * 4;
			indices[indexCounter + 5] = 3 + i * 4;
		}

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = indices;
		Config::Device->CreateBuffer(&ibDesc, &data, &indexBuffer);

		delete[] indices;
	}

	// PARTICLE POOL ============
	{
		// Buffer
		ID3D11Buffer* particlePoolBuffer;
		D3D11_BUFFER_DESC poolDesc = {};
		poolDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		poolDesc.ByteWidth = sizeof(Particle) * maxParticles;
		poolDesc.CPUAccessFlags = 0;
		poolDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		poolDesc.StructureByteStride = sizeof(Particle);
		poolDesc.Usage = D3D11_USAGE_DEFAULT;
		Config::Device->CreateBuffer(&poolDesc, 0, &particlePoolBuffer);

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC poolUAVDesc = {};
		poolUAVDesc.Format = DXGI_FORMAT_UNKNOWN; // Needed for RW structured buffers
		poolUAVDesc.Buffer.FirstElement = 0;
		poolUAVDesc.Buffer.Flags = 0;
		poolUAVDesc.Buffer.NumElements = maxParticles;
		poolUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		Config::Device->CreateUnorderedAccessView(particlePoolBuffer, &poolUAVDesc, &particlePoolUAV);

		// SRV (for indexing in VS)
		D3D11_SHADER_RESOURCE_VIEW_DESC poolSRVDesc = {};
		poolSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		poolSRVDesc.Buffer.FirstElement = 0;
		poolSRVDesc.Buffer.NumElements = maxParticles;
		// Don't actually set these!  They're union'd with above data, so 
		// it will just overwrite correct values with incorrect values
		//poolSRVDesc.Buffer.ElementOffset = 0;
		//poolSRVDesc.Buffer.ElementWidth = sizeof(Particle);
		poolSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		Config::Device->CreateShaderResourceView(particlePoolBuffer, &poolSRVDesc, &particlePoolSRV);

		// Done
		particlePoolBuffer->Release();
	}

	// DEAD LIST ===================
	{
		// Buffer
		ID3D11Buffer* deadListBuffer;
		D3D11_BUFFER_DESC deadDesc = {};
		deadDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		deadDesc.ByteWidth = sizeof(unsigned int) * maxParticles;
		deadDesc.CPUAccessFlags = 0;
		deadDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		deadDesc.StructureByteStride = sizeof(unsigned int);
		deadDesc.Usage = D3D11_USAGE_DEFAULT;
		Config::Device->CreateBuffer(&deadDesc, 0, &deadListBuffer);

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC deadUAVDesc = {};
		deadUAVDesc.Format = DXGI_FORMAT_UNKNOWN; // Needed for RW structured buffers
		deadUAVDesc.Buffer.FirstElement = 0;
		deadUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND; // Append/Consume
		deadUAVDesc.Buffer.NumElements = maxParticles;
		deadUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		Config::Device->CreateUnorderedAccessView(deadListBuffer, &deadUAVDesc, &particleDeadUAV);

		// Done
		deadListBuffer->Release();
	}

	// Draw List
	{
		// Buffer
		ID3D11Buffer* drawListBuffer;
		D3D11_BUFFER_DESC drawDesc = {};
		drawDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		drawDesc.ByteWidth = sizeof(ParticleDrawInfo) * maxParticles;
		drawDesc.CPUAccessFlags = 0;
		drawDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		drawDesc.StructureByteStride = sizeof(ParticleDrawInfo);
		drawDesc.Usage = D3D11_USAGE_DEFAULT;
		Config::Device->CreateBuffer(&drawDesc, 0, &drawListBuffer);

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC drawUAVDesc = {};
		drawUAVDesc.Format = DXGI_FORMAT_UNKNOWN; // Needed for RW structured buffers
		drawUAVDesc.Buffer.FirstElement = 0;
		drawUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER; // IncrementCounter() in HLSL
		drawUAVDesc.Buffer.NumElements = maxParticles;
		drawUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		Config::Device->CreateUnorderedAccessView(drawListBuffer, &drawUAVDesc, &particleDrawUAV);

		// SRV (for indexing in VS)
		D3D11_SHADER_RESOURCE_VIEW_DESC drawSRVDesc = {};
		drawSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		drawSRVDesc.Buffer.FirstElement = 0;
		drawSRVDesc.Buffer.NumElements = maxParticles;
		// Don't actually set these!  They're union'd with above data, so 
		// it will just overwrite correct values with incorrect values
		//drawSRVDesc.Buffer.ElementOffset = 0;
		//drawSRVDesc.Buffer.ElementWidth = sizeof(ParticleSort);
		drawSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		Config::Device->CreateShaderResourceView(drawListBuffer, &drawSRVDesc, &particleDrawSRV);

		// Done
		drawListBuffer->Release();
	}

	// DRAW ARGS ================
	{
		// Buffer
		D3D11_BUFFER_DESC argsDesc = {};
		argsDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		argsDesc.ByteWidth = sizeof(unsigned int) * 5; // Need 5 if using an index buffer!
		argsDesc.CPUAccessFlags = 0;
		argsDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		argsDesc.StructureByteStride = 0;
		argsDesc.Usage = D3D11_USAGE_DEFAULT;
		Config::Device->CreateBuffer(&argsDesc, 0, &drawArgsBuffer);

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC argsUAVDesc = {};
		argsUAVDesc.Format = DXGI_FORMAT_R32_UINT; // Actually UINT's in here!
		argsUAVDesc.Buffer.FirstElement = 0;
		argsUAVDesc.Buffer.Flags = 0;  // Nothing special
		argsUAVDesc.Buffer.NumElements = 5; // Need 5 if using an index buffer
		argsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		Config::Device->CreateUnorderedAccessView(drawArgsBuffer, &argsUAVDesc, &drawArgsUAV);

		// Must keep buffer ref for indirect draw!
	}

	//Random numbers
	{
		// Buffer
		ID3D11Buffer* randBuffer;
		D3D11_BUFFER_DESC randDesc = {};
		randDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		randDesc.ByteWidth = sizeof(float) * maxParticles * RAND_CNT;
		randDesc.CPUAccessFlags = 0;
		randDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		randDesc.StructureByteStride = sizeof(float);
		randDesc.Usage = D3D11_USAGE_DEFAULT;
		Config::Device->CreateBuffer(&randDesc, 0, &randBuffer);

		// UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC randUAVDesc = {};
		randUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		randUAVDesc.Buffer.FirstElement = 0;
		randUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		randUAVDesc.Buffer.NumElements = maxParticles * RAND_CNT;
		randUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		Config::Device->CreateUnorderedAccessView(randBuffer, &randUAVDesc, &randUAV);
		
		randBuffer->Release();
	}

	// Launch the dead list init shader
	defaultShaders.initDeadListCS->SetInt("maxParticles", maxParticles);
	defaultShaders.initDeadListCS->SetUnorderedAccessView("DeadParticles", particleDeadUAV);
	defaultShaders.initDeadListCS->SetShader();
	defaultShaders.initDeadListCS->CopyAllBufferData();
	defaultShaders.initDeadListCS->DispatchByThreads(maxParticles, 1, 1);

	// Additive blend state
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; 
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	Config::Device->CreateBlendState(&blend, &additiveBlend);

	D3D11_DEPTH_STENCIL_DESC depth = {};
	depth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth.DepthFunc = D3D11_COMPARISON_LESS;
	depth.DepthEnable = true;
	Config::Device->CreateDepthStencilState(&depth, &depthWriteOff);
}

void GPUParticleEmitter::SetCustomShaders(SimpleComputeShader* emitParticle, SimpleComputeShader* updateParticle)
{
	customShaders.particleEmissionCS = emitParticle;
	customShaders.particleUpdateCS = updateParticle;
	customShadersEnabled = true;
}

void GPUParticleEmitter::SetBlendingEnabled(bool toggle)
{
	this->blendingEnabled = toggle;
}

void GPUParticleEmitter::Update(double deltaTime, double totalTime, XMFLOAT4X4 view)
{
	ParticleEmitter::Update(deltaTime, totalTime);

	SimpleComputeShader* emitCS = customShadersEnabled ? customShaders.particleEmissionCS : defaultShaders.particleEmissionCS;
	SimpleComputeShader* updateCS = customShadersEnabled ? customShaders.particleUpdateCS : defaultShaders.particleUpdateCS;
	SimpleComputeShader* randCS = customShadersEnabled ? customShaders.randomNumsCS : defaultShaders.randomNumsCS;

	// Reset UAVs (potential issue with setting the following ones)
	Config::Context->CSSetUnorderedAccessViews(0, 8, noneUAV, 0);

	// Track time
	while ((emitTimeCounter >= emissionRate) && isActive)
	{
		// How many to emit?
		int emitCount = (int)(emitTimeCounter / emissionRate);

		// Max to emit in a single batch is 65,535
		emitCount = min(emitCount, 65535);

		randCS->SetShader();
		randCS->SetFloat("seed1", static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
		randCS->SetFloat("seed2", static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
		randCS->SetInt("count", emitCount * RAND_CNT);
		//randCS->SetInt("maxParticles", (int)maxParticles);
		randCS->SetUnorderedAccessView("RandNums", randUAV);
		//randCS->SetUnorderedAccessView("DeadParticles", particleDeadUAV);
		randCS->CopyAllBufferData();
		randCS->DispatchByThreads(emitCount * RAND_CNT, 1, 1);

		Config::Context->CSSetUnorderedAccessViews(0, 8, noneUAV, 0);

		// Adjust time counter
		emitTimeCounter = fmod(emitTimeCounter, emissionRate);

		// Emit an appropriate amount of particles
		emitCS->SetShader();
		//ParticleColor colors[MAX_COLORS];
		emitCS->SetFloat("totalTime", totalTime);
		emitCS->SetInt("emitCount", emitCount);
		emitCS->SetInt("maxParticles", (int)maxParticles);
		emitCS->SetInt("colorCount", colorCount);
		emitCS->SetInt("textureCount", textureCount);

		emitCS->SetFloat("emissionStartRadius", emissionStartRadius);
		emitCS->SetFloat("emissionEndRadius", emissionEndRadius);
		emitCS->SetFloat("particleMinLifetime", particleMinLifetime);
		emitCS->SetFloat("particleMaxLifetime", particleMaxLifetime);
		emitCS->SetFloat("particleInitMinScale", particleInitMinScale);
		emitCS->SetFloat("particleInitMaxScale", particleInitMaxScale);
		emitCS->SetFloat("particleInitMinAngularVelocity", particleInitMinAngularVelocity);
		emitCS->SetFloat("particleInitMaxAngularVelocity", particleInitMaxAngularVelocity);
		emitCS->SetFloat("particleInitMinSpeed", particleInitMinSpeed);
		emitCS->SetFloat("particleInitMaxSpeed", particleInitMaxSpeed);
		emitCS->SetMatrix4x4("world", worldMatrix);
		emitCS->SetInt("bakeWorldMat", (int)bakeWorldMatOnEmission);

		emitCS->SetFloat3("particleAcceleration", particleAcceleration);

		emitCS->SetData("colors", colors, sizeof(ParticleColor) * MAX_PARTICLE_COLORS);
		emitCS->SetData("textures", texturesToGPU, sizeof(ParticleTextureToGPU) * MAX_PARTICLE_TEXTURES);
		emitCS->SetUnorderedAccessView("ParticlePool", particlePoolUAV);
		emitCS->SetUnorderedAccessView("DeadParticles", particleDeadUAV);
		emitCS->SetUnorderedAccessView("RandNums", randUAV);
		emitCS->CopyAllBufferData();
		emitCS->DispatchByThreads(emitCount, 1, 1);
	}

	Config::Context->CSSetUnorderedAccessViews(0, 8, noneUAV, 0);

	// Update
	updateCS->SetShader();
	updateCS->SetFloat("deltaTime", deltaTime);
	updateCS->SetFloat("totalTime", totalTime);
	updateCS->SetFloat("lifetime", lifetime);
	updateCS->SetInt("maxParticles", maxParticles);
	updateCS->SetFloat("fadeOutStartTime", fadeOutStartTime);
	updateCS->SetInt("fadeOut", (int)fadeOut);
	updateCS->SetFloat("fadeInEndTime", fadeInEndTime);
	updateCS->SetInt("fadeIn", (int)fadeIn);
	updateCS->SetFloat("particleAvgLifetime", particleAvgLifetime);
	//updateCS->SetFloat3("cameraPos", XMFLOAT3(view._14, view._24, view._34));
	updateCS->SetUnorderedAccessView("ParticlePool", particlePoolUAV);
	updateCS->SetUnorderedAccessView("DeadParticles", particleDeadUAV);
	updateCS->SetUnorderedAccessView("ParticleDrawList", particleDrawUAV, 0); // Reset counter for update!

	updateCS->CopyAllBufferData();
	updateCS->DispatchByThreads(maxParticles, 1, 1);

	// Binding order issues with next stage, so just reset here
	Config::Context->CSSetUnorderedAccessViews(0, 8, noneUAV, 0);

	// Get draw data
	defaultShaders.copyDrawCountCS->SetShader();
	defaultShaders.copyDrawCountCS->SetUnorderedAccessView("DrawArguments", drawArgsUAV);
	defaultShaders.copyDrawCountCS->SetUnorderedAccessView("ParticleDrawList", particleDrawUAV); // Don't reset counter!!!
	defaultShaders.copyDrawCountCS->DispatchByThreads(1, 1, 1);

	// Reset here too
	Config::Context->CSSetUnorderedAccessViews(0, 8, noneUAV, 0);
}

void GPUParticleEmitter::Draw(XMFLOAT4X4 view, XMFLOAT4X4 proj)
{
	if (blendingEnabled)
	{
		Config::Context->OMSetBlendState(additiveBlend, 0, 0xFFFFFFFF);
		Config::Context->OMSetDepthStencilState(depthWriteOff, 0);
	}

	// Assuming triangles, set index buffer
	Config::Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Gotta do it manually! SimpleShader doesn't currently
	// handle structured buffers in Vertex Shaders
	Config::Context->VSSetShaderResources(0, 1, &particlePoolSRV);
	Config::Context->VSSetShaderResources(1, 1, &particleDrawSRV);

	defaultShaders.particleVS->SetShader();
	defaultShaders.particleVS->SetMatrix4x4("world", worldMatrix);
	defaultShaders.particleVS->SetMatrix4x4("view", view);
	defaultShaders.particleVS->SetMatrix4x4("projection", proj);
	defaultShaders.particleVS->CopyAllBufferData();

	defaultShaders.particlePS->SetShader();
	defaultShaders.particlePS->SetShaderResourceView("particleTextures", texturesSRV);
	defaultShaders.particlePS->SetSamplerState("Sampler", Config::Sampler);

	// Draw using indirect args
	Config::Context->DrawIndexedInstancedIndirect(drawArgsBuffer, 0);

	Config::Context->VSSetShaderResources(0, 16, noneSRV);

	defaultShaders.particlePS->SetShaderResourceView("particleTextures", NULL);

	if (blendingEnabled)
	{
		Config::Context->OMSetBlendState(0, 0, 0xFFFFFFFF);
		Config::Context->OMSetDepthStencilState(0, 0);
	}
}