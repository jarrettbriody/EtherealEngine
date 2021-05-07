#include "pch.h"
#include "FireProjectile.h"

void FireProjectile::OnInitialize()
{
	projectile->GetRBody()->setGravity(btVector3(0, 0, 0));
	// projectile->GetRBody()->setCollisionFlags(projectile->GetRBody()->getCollisionFlags() | btRigidBody::CF_NO_CONTACT_RESPONSE);


	XMFLOAT3 pos = enemy->GetPosition();
	pos.y = pos.y + 10.0f;
	projectile->SetPosition(pos);

	XMFLOAT3 playerPos = player->GetPosition();
	XMFLOAT3 enemyPos = enemy->GetPosition();
	
	XMFLOAT3 projectileDirection;
	XMStoreFloat3(&projectileDirection, XMVectorSubtract(XMLoadFloat3(&enemyPos), XMLoadFloat3(&playerPos)));
	
	btVector3 enemyCenterOfMass = enemy->GetRBody()->getCenterOfMassPosition();
	btVector3 playerCenterOfMass = player->GetRBody()->getCenterOfMassPosition();

	projectile->GetRBody()->activate();
	projectile->GetRBody()->setLinearVelocity((enemyCenterOfMass - playerCenterOfMass).normalized() * projectileSpeed);
}

void FireProjectile::OnTerminate(Status s)
{
}

Status FireProjectile::Update()
{
	/*XMFLOAT3 pos;
	XMStoreFloat3(&pos, XMVectorLerp(XMLoadFloat3(&projectile->GetPosition()), XMLoadFloat3(&player->GetPosition()), deltaTime * projectileSpeed));
	projectile->SetPosition(pos);

	XMFLOAT3 tolerance = XMFLOAT3(0.1f, 0.1f, 0.1f);

	if (XMVector3NearEqual(XMLoadFloat3(&pos), XMLoadFloat3(&player->GetPosition()), XMLoadFloat3(&tolerance)))
	{
		return SUCCESS;
	}
	else
	{
		return FAILURE;
	}*/
	
	return SUCCESS;
}
