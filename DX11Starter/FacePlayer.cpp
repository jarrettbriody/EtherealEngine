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
	XMVECTOR dir = XMLoadFloat3(&enemy->GetDirectionVector());
	XMFLOAT3 pos = enemy->GetPosition();
	XMFLOAT3 playerPos = player->GetPosition();
	XMVECTOR tarDir = XMVectorSet(playerPos.x - pos.x, 0.0f, playerPos.z - pos.z, 1.0f);
	XMVECTOR newVec = XMVectorLerp(dir, tarDir, turnSpeed * turnSpeed);

	enemy->SetDirectionVector(XMFLOAT3(XMVectorGetX(newVec), 0.0f, XMVectorGetZ(newVec)));
	enemy->CalcWorldMatrix();

	return SUCCESS;
}