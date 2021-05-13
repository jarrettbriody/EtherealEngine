#include "pch.h"
#include "BodyPart.h"

void BodyPart::Init()
{
	ParticleEmitterDescription emitDesc;
	//emitDesc.parentName = "FPSController";
	//emitDesc.parentWorld = EESceneLoader->sceneEntitiesMap["FPSController"]->GetWorldMatrixPtr();
	emitDesc.emitterPosition = XMFLOAT3(0, 0.0f, 0);

	ParticleColor partColors[3] = {
		{XMFLOAT4(1.0f,0,0,0.5f),5.0f},
		{XMFLOAT4(1.0f,0,0,0.5f),5.0f},
		{XMFLOAT4(1.0f,0,0,0.2f),5.0f},
	};
	emitDesc.colors = partColors;
	emitDesc.colorCount = 3;
	emitDesc.maxParticles = 100;
	emitDesc.emissionRate = 2.0f;
	//emitDesc.emissionRotation = XMFLOAT3(-XM_PIDIV2,0.0f,0.0f);
	//emitDesc.parentName = entity->GetName();
	emitDesc.parentWorld = entity->GetTransform().GetWorldMatrixPtr();
	emitDesc.emitterDirection = Y_AXIS;
	emitDesc.particleInitMinSpeed = 15.0f;
	emitDesc.particleInitMaxSpeed = 20.0f;
	emitDesc.particleMinLifetime = 10.0f;
	emitDesc.particleMaxLifetime = 15.0f;
	//emitDesc.particleInitMinScale = 0.1f;
	//emitDesc.particleInitMaxScale = 0.2f;
	emitDesc.emissionStartRadius = 0.1f;
	emitDesc.emissionEndRadius = 0.6f;
	emitDesc.particleInitMinScale = 0.1f;
	emitDesc.particleInitMaxScale = 0.11f;
	emitDesc.particleAcceleration = XMFLOAT3(0.0f,-20.0f, 0.0f);
	emitDesc.bakeWorldMatOnEmission = true;

	cpuPSys = new CPUParticleEmitter(emitDesc);
	//cpuEmitter->SetBlendingEnabled(true);

	cpuPSys->SetCollisionsEnabled([](void* collision) {
		btPersistentManifold* manifold = (btPersistentManifold*)collision;
		btCollisionObject* obA = (btCollisionObject*)(manifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(manifold->getBody1());

		PhysicsWrapper* wrapperA = (PhysicsWrapper*)obA->getUserPointer();
		PhysicsWrapper* wrapperB = (PhysicsWrapper*)obB->getUserPointer();

		if (wrapperA->type == PHYSICS_WRAPPER_TYPE::PARTICLE && wrapperB->type == PHYSICS_WRAPPER_TYPE::ENTITY ||
			wrapperB->type == PHYSICS_WRAPPER_TYPE::PARTICLE && wrapperA->type == PHYSICS_WRAPPER_TYPE::ENTITY) {

			ParticlePhysicsWrapper* particleWrapper = (wrapperA->type == PHYSICS_WRAPPER_TYPE::PARTICLE) ? (ParticlePhysicsWrapper*)wrapperA->objectPointer : (ParticlePhysicsWrapper*)wrapperB->objectPointer;
			Entity* entity = (wrapperA->type == PHYSICS_WRAPPER_TYPE::ENTITY) ? (Entity*)wrapperA->objectPointer : (Entity*)wrapperB->objectPointer;

			if (entity->HasLayer("decal") && !entity->destroyed) {
				//cout << entity->GetName() << endl;

				Particle* particle = (Particle*)particleWrapper->particle;
				CPUParticleEmitter* emitter = (CPUParticleEmitter*)particleWrapper->emitter;
				ParticleEmitter* emitterBase = (ParticleEmitter*)emitter;

				XMFLOAT3 particleWorld = particle->position;
				XMFLOAT3 normalizedVel = particle->velocity;
				XMStoreFloat3(&normalizedVel, XMVector3Normalize(XMLoadFloat3(&normalizedVel)));

				if (!particle->worldMatBaked) {
					XMVECTOR particlePos = XMLoadFloat3(&particle->position);
					XMMATRIX emitterWorld = XMMatrixTranspose(XMLoadFloat4x4(&emitterBase->GetWorldMatrix()));
					particlePos.m128_f32[3] = 1.0f;
					XMStoreFloat3(&particleWorld, XMVector4Transform(particlePos, emitterWorld));

					XMVECTOR particleVel = XMLoadFloat3(&particle->velocity);
					particleVel.m128_f32[3] = 0.0f;
					XMStoreFloat3(&normalizedVel, XMVector3Normalize(XMVector4Transform(particleVel, emitterWorld)));
				}
				float scaleRand = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				XMFLOAT3 decalScale(6.0f + 4.0f * scaleRand, 6.0f + 4.0f * scaleRand, 15.0f);
				DecalHandler::GetInstance()->GenerateDecal(entity, normalizedVel, particleWorld, decalScale, DecalType(rand() % 8));

				emitter->KillParticle(particleWrapper->particleIndex);
				//Config::DynamicsWorld->getDispatcher()->clearManifold(manifold);
			}
		}
		});

	emitDesc.maxParticles = 6000;
	emitDesc.emissionRate = 400.0f;
	emitDesc.particleMinLifetime = 3.0f;
	emitDesc.particleMaxLifetime = 4.0f;
	gpuPSys = new GPUParticleEmitter(emitDesc);
}

void BodyPart::Update()
{
	totalTime += deltaTime;
	if (totalTime > deactivate && !destroyedPSys) {
		ParticleEmitter* emit = (ParticleEmitter*)cpuPSys;
		emit->SetIsActive(false);
		emit = (ParticleEmitter*)gpuPSys;
		emit->SetIsActive(false);
	}
	if (totalTime > timeout && !destroyedPSys) {
		ParticleEmitter* emit = (ParticleEmitter*)cpuPSys;
		emit->Destroy();
		emit = (ParticleEmitter*)gpuPSys;
		emit->Destroy();
		destroyedPSys = true;
	}
}
