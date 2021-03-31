#include "pch.h"
#include "CPUParticleEmitter.h"

DefaultCPUParticleShaders CPUParticleEmitter::defaultShaders;

void CPUParticleEmitter::SetDefaultShaders(DefaultCPUParticleShaders s)
{
	defaultShaders = s;
}

CPUParticleEmitter::CPUParticleEmitter() : ParticleEmitter()
{
	InitBuffers();
}

CPUParticleEmitter::CPUParticleEmitter(ParticleEmitterDescription d, bool blendingEnabled) : ParticleEmitter(d)
{
	this->blendingEnabled = blendingEnabled;
	InitBuffers();
}

CPUParticleEmitter::~CPUParticleEmitter()
{
	indexBuffer->Release();
	vertexBuffer->Release();
	additiveBlend->Release();
	depthWriteOff->Release();
	delete[] particlePool;
	delete[] particleVertices;
	delete[] deadList;
}

void CPUParticleEmitter::InitBuffers()
{
	// DYNAMIC vertex buffer (no initial data necessary)
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * maxParticles;
	Config::Device->CreateBuffer(&vbDesc, 0, &vertexBuffer);

	// Index buffer data
	unsigned int* indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	// Regular (static) index buffer
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;
	Config::Device->CreateBuffer(&ibDesc, &indexData, &indexBuffer);

	delete[] indices;

	particlePool = new Particle[maxParticles];
	ZeroMemory(particlePool, sizeof(Particle) * maxParticles);
	deadList = new unsigned int[maxParticles];
	particleVertices = new ParticleVertex[4 * maxParticles];
	particleVertCount = 0;
	drawCount = 0;
	deadListCount = maxParticles;

	for (size_t i = 0; i < maxParticles; i++)
	{
		deadList[i] = i;
		particleVertices[i + 0].UV = XMFLOAT2(0, 0);
		particleVertices[i + 1].UV = XMFLOAT2(1, 0);
		particleVertices[i + 2].UV = XMFLOAT2(1, 1);
		particleVertices[i + 3].UV = XMFLOAT2(0, 1);

		particleVertices[i + 0].ID = 0;
		particleVertices[i + 1].ID = 1;
		particleVertices[i + 2].ID = 2;
		particleVertices[i + 3].ID = 3;
	}

	// Additive blend state
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
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

void CPUParticleEmitter::CalcVertex(Particle p, XMFLOAT4X4 view)
{
	particleVertices[particleVertCount].Position = p.position;
	particleVertices[particleVertCount].Color = p.color;
	particleVertices[particleVertCount].Scale = p.scale;
	particleVertCount++;

	particleVertices[particleVertCount].Position = p.position;
	particleVertices[particleVertCount].Color = p.color;
	particleVertices[particleVertCount].Scale = p.scale;
	particleVertCount++;

	particleVertices[particleVertCount].Position = p.position;
	particleVertices[particleVertCount].Color = p.color;
	particleVertices[particleVertCount].Scale = p.scale;
	particleVertCount++;

	particleVertices[particleVertCount].Position = p.position;
	particleVertices[particleVertCount].Color = p.color;
	particleVertices[particleVertCount].Scale = p.scale;
	particleVertCount++;
}

void CPUParticleEmitter::SetBlendingEnabled(bool toggle)
{
	this->blendingEnabled = toggle;
}

void CPUParticleEmitter::Update(float deltaTime, float totalTime, XMFLOAT4X4 view)
{
	ParticleEmitter::Update(deltaTime, totalTime);

	particleVertCount = 0;
	drawCount = 0;

	// Track time
	while (emitTimeCounter >= emissionRate)
	{
		// How many to emit?
		int emitCount = (int)(emitTimeCounter / emissionRate);

		// Max to emit in a single batch is 65,535
		emitCount = min(emitCount, 65535);

		// Adjust time counter
		emitTimeCounter = fmod(emitTimeCounter, emissionRate);

		float randNum;
		float randNum2;
		float randNum3;
		float randNum4;
		float randNum5;
		unsigned int newParticleIndex;
		float speed;
		float randomOffset;
		float randomOffset2;
		XMFLOAT3 start;
		XMFLOAT3 end;

		for (size_t i = 0; i < emitCount; i++)
		{
			randNum = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			randNum2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			randNum3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			randNum4 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			randNum5 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			// Grab a single index from the dead list
			if (deadListCount == 0) break;
			newParticleIndex = deadList[deadListCount - 1];
			deadListCount--;

			// Update it in the particle pool
			Particle newParticle;

			newParticle.color = XMFLOAT4(1, 0, 0, 1);

			// Color and position depend on the grid position and size
			for (int j = colorCount - 1; j >= 0; j--)
			{
				if (randNum3 <= colors[j].weight) {
					newParticle.color = colors[j].color;
					break;
				}
			}

			speed = particleInitMinSpeed + (particleInitMaxSpeed - particleInitMinSpeed) * randNum5;
			randomOffset = randNum * 2.0f - 1.0f;
			randomOffset2 = randNum2 * 2.0f - 1.0f;

			start = XMFLOAT3(randomOffset * emissionStartRadius, randomOffset2 * emissionStartRadius, 0.0f);
			end = XMFLOAT3(randomOffset * emissionEndRadius, randomOffset2 * emissionEndRadius, 1.0f);

			newParticle.remainingLife = particleMinLifetime + (particleMaxLifetime - particleMinLifetime) * randNum;
			newParticle.position = XMFLOAT3(0, 0, 0);
			newParticle.scale = particleInitMinScale + (particleInitMaxScale - particleInitMinScale) * randNum4;
			DirectX::XMStoreFloat3(&newParticle.velocity, XMVectorScale(XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&end), XMLoadFloat3(&start))), speed));
			newParticle.rotationRadians = 0.0f;
			newParticle.angularVelocity = particleInitMinAngularVelocity + (particleInitMaxAngularVelocity - particleInitMinAngularVelocity) * randNum;
			newParticle.acceleration = particleAcceleration;

			// Put it back
			particlePool[newParticleIndex] = newParticle;
			//drawList[drawListCount] = newParticleIndex;
			//drawListCount++;
		}
	}

	for (size_t i = 0; i < maxParticles; i++)
	{
		Particle particle = particlePool[i];

		// Early out for ALREADY DEAD particles (so they don't go back on dead list)
		if (particle.remainingLife <= 0.0f)	continue;

		// Update the particle
		particle.remainingLife -= deltaTime;
		XMVECTOR vel = XMLoadFloat3(&particle.velocity);
		DirectX::XMStoreFloat3(&particle.velocity, XMVectorAdd(vel, XMVectorScale(XMLoadFloat3(&particle.acceleration), deltaTime)));
		DirectX::XMStoreFloat3(&particle.position, XMVectorAdd(XMLoadFloat3(&particle.position),XMVectorScale(vel,deltaTime)));
		particle.rotationRadians += particle.angularVelocity * deltaTime;

		// Put the particle back
		particlePool[i] = particle;

		// Newly dead?
		if (particle.remainingLife <= 0.0f)
		{
			// Add to dead list
			deadList[deadListCount] = i;
			deadListCount++;
		}
		else
		{
			CalcVertex(particle, view);
			drawCount++;
		}
	}
}

void CPUParticleEmitter::Draw(XMFLOAT4X4 view, XMFLOAT4X4 proj)
{
	if (blendingEnabled)
	{
		Config::Context->OMSetBlendState(additiveBlend, 0, 0xFFFFFFFF);
		Config::Context->OMSetDepthStencilState(depthWriteOff, 0);
	}

	// Copy to dynamic buffer
	// All particles copied locally - send whole buffer to GPU
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	Config::Context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, particleVertices, sizeof(ParticleVertex) * 4 * maxParticles);
	Config::Context->Unmap(vertexBuffer, 0);

	// Set up buffers
	UINT stride = sizeof(ParticleVertex);
	UINT offset = 0;
	Config::Context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	Config::Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	defaultShaders.particleVS->SetMatrix4x4("world", worldMatrix);
	defaultShaders.particleVS->SetMatrix4x4("view", view);
	defaultShaders.particleVS->SetMatrix4x4("projection", proj);
	defaultShaders.particleVS->SetShader();
	defaultShaders.particleVS->CopyAllBufferData();

	//defaultShaders.particlePS->SetShaderResourceView("particle", texture);
	defaultShaders.particlePS->SetShader();

	// Draw the correct parts of the buffer
	Config::Context->DrawIndexed(drawCount * 6, 0, 0);

	if (blendingEnabled)
	{
		Config::Context->OMSetBlendState(0, 0, 0xFFFFFFFF);
		Config::Context->OMSetDepthStencilState(0, 0);
	}
}

