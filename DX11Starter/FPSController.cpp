#include "FPSController.h"

void FPSController::Init()
{
	cam = ScriptManager::EERenderer->GetCamera("main");
	direction = cam->direction;
}

void FPSController::Update()
{
	direction = cam->direction;
	XMFLOAT3 yAxis = Y_AXIS;
	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction); 
	dir.m128_f32[1] = 0.0f;
	XMVECTOR right = XMVector3Cross(dir, XMLoadFloat3(&yAxis));

	/*
	if (GetAsyncKeyState('W') & 0x8000) {
		pos = XMVectorAdd(pos, XMVectorScale(dir, 0.05f));
		XMStoreFloat3(&position, pos);
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		pos = XMVectorAdd(pos, XMVectorScale(dir, -0.05f));
		XMStoreFloat3(&position, pos);
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		pos = XMVectorAdd(pos, XMVectorScale(right, 0.05f));
		XMStoreFloat3(&position, pos);
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		pos = XMVectorAdd(pos, XMVectorScale(right, -0.05f));
		XMStoreFloat3(&position, pos);
	}
	cam->SetPosition(position);
	*/
}

void FPSController::OnMouseMove(WPARAM buttonState, int x, int y)
{
	if (buttonState & 0x0001) {
		cam->RotateCamera(x - (int)prevMousePos.x, y - (int)prevMousePos.y);

		prevMousePos.x = x;
		prevMousePos.y = y;
	}
}
