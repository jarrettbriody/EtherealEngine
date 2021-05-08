#include "pch.h"
#include "FacePlayer.h"

void FacePlayer::OnInitialize()
{
}

void FacePlayer::OnTerminate(Status s)
{
}

Status FacePlayer::Update()
{
	XMVECTOR dir = XMLoadFloat3(&enemy->GetTransform().GetDirectionVector());
	XMFLOAT3 pos = enemy->GetTransform().GetPosition();
	XMFLOAT3 playerPos = player->GetTransform().GetPosition();
	XMVECTOR diff = XMVectorSubtract(XMLoadFloat3(&playerPos), XMLoadFloat3(&pos));
	diff.m128_f32[1] = 0.0f;
	diff.m128_f32[3] = 0.0f;
	XMVECTOR tarDir = XMVector3Normalize(diff);
	XMVECTOR newVec = XMVector3Normalize(XMVectorLerp(dir, tarDir, speed * (*deltaTime)));
	XMFLOAT3 newDirection;
	XMStoreFloat3(&newDirection, newVec);
	enemy->GetTransform().SetDirectionVector(newDirection);

	return SUCCESS;
}