#include "pch.h"
#include "CPUParticleEmitter.h"

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
	additiveBlend->Release();
	depthWriteOff->Release();
}

void CPUParticleEmitter::InitBuffers()
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

	particlePool = new Particle[maxParticles];
	drawList = new unsigned int[maxParticles];
	deadList = new unsigned int[maxParticles];
	drawListCount = 0;
	deadListCount = maxParticles;

	for (size_t i = 0; i < maxParticles; i++)
	{
		deadList[i] = i;
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

void CPUParticleEmitter::SetBlendingEnabled(bool toggle)
{
	this->blendingEnabled = toggle;
}

void CPUParticleEmitter::Update(float deltaTime, float totalTime)
{
	ParticleEmitter::Update(deltaTime, totalTime);

	drawListCount = 0;

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
			deadList[deadListCount];
			deadListCount++;
		}
		else
		{
			drawList[drawListCount] = i;
			drawListCount++;
		}
	}
}

void CPUParticleEmitter::Draw(XMFLOAT4X4 view, XMFLOAT4X4 proj)
{

}

