#include "pch.h"
#include "Projectile.h"

void Projectile::Init()
{
	eMap = ScriptManager::sceneEntitiesMap;

	ParticleEmitterDescription emitDesc;
	emitDesc.emitterPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	emitDesc.parentName = entity->GetName();
	emitDesc.parentWorld = entity->GetTransform().GetWorldMatrixPtr();//&emitterTransform;//
	emitDesc.emitterDirection = NEG_Y_AXIS;
	emitDesc.colorCount = 1;
	ParticleColor particleColors[1] = {
		{XMFLOAT4(0.45f, 0.0f, 0.0f, 0.7f), 1.0f},
	};
	emitDesc.colors = particleColors;
	emitDesc.bakeWorldMatOnEmission = true;
	emitDesc.emissionStartRadius = 0.1f;
	emitDesc.emissionEndRadius = 0.20f;
	emitDesc.emissionRate = 30.0;
	emitDesc.maxParticles = 200;
	emitDesc.particleInitMinSpeed = 10.0f;
	emitDesc.particleInitMaxSpeed = 20.0f;
	emitDesc.particleMinLifetime = 2.0f;
	emitDesc.particleMaxLifetime = 3.0f;
	emitDesc.particleInitMinScale = 0.1f;
	emitDesc.particleInitMaxScale = 0.2f;
	//emitDesc.fadeInEndTime = 0.1f;
	//emitDesc.fadeIn = true;
	emitDesc.fadeOutStartTime = 0.5f;
	emitDesc.fadeOut = true;
	emitDesc.particleAcceleration = XMFLOAT3(0, -15.0f, 0);

	new GPUParticleEmitter(emitDesc);
}

void Projectile::Update()
{
}

void Projectile::OnCollision(btCollisionObject* other)
{
	PhysicsWrapper* wrapper = (PhysicsWrapper*)other->getUserPointer();

	if (wrapper->type == PHYSICS_WRAPPER_TYPE::ENTITY)
	{
		Entity* otherE = (Entity*)wrapper->objectPointer;

		if (otherE->HasTag("Environment"))
		{
			entity->Destroy();
		}
	}
}
