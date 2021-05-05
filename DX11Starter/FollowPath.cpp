#include "pch.h"
#include "FollowPath.h"

void FollowPath::OnInitialize()
{
}

void FollowPath::OnTerminate(Status s)
{
}

Status FollowPath::Update()
{
	if (path->size() == 0)
		return FAILURE;

	XMFLOAT3 pos = enemy->GetPosition();
	XMFLOAT3 targetPos = path->front()->GetPos();

	float distance = sqrt(pow(targetPos.x - pos.x, 2) + pow(targetPos.z - pos.z, 2));

	if (distance < minimumDistance)
	{
		//path->pop_front();
		path->erase(path->begin());
		if (path->size() == 0)
			return SUCCESS;
		
		targetPos = path->front()->GetPos();
	}

	movementDirection = btVector3(targetPos.x - pos.x, 0.0f, targetPos.z - pos.z);

	// FORCES ADDED TO RIGIDBODY
	enemy->GetRBody()->activate();
	enemy->GetRBody()->setLinearVelocity(movementDirection.normalized() * movementSpeed);

	// Look where we are going
	XMVECTOR dir = XMLoadFloat3(&enemy->GetDirectionVector());
	XMVECTOR diff = XMVectorSubtract(XMLoadFloat3(&targetPos), XMLoadFloat3(&pos));
	diff.m128_f32[1] = 0.0f;
	diff.m128_f32[3] = 0.0f;
	XMVECTOR tarDir = XMVector3Normalize(diff);
	XMVECTOR newVec = XMVector3Normalize(XMVectorLerp(dir, tarDir, turningSpeed * (*deltaTime)));
	XMFLOAT3 newDirection;
	XMStoreFloat3(&newDirection, newVec);
	enemy->SetDirectionVector(newDirection);
	enemy->CalcWorldMatrix();

	return RUNNING;
}