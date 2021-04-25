#include "pch.h"
#include "Camera.h"


Camera::Camera()
{
	position = XMFLOAT3(0, 10, 0);
	direction = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMVECTOR dir = XMLoadFloat3(&direction);
	dir = XMVector3Normalize(dir);
	XMStoreFloat3(&direction, dir);
	xRotation = 0.0f;
	yRotation = 0.0f;
	zRotation = 0.0f;
	mouse = Mouse::GetInstance();
	prevMousePos = { 0, 0 };
}


Camera::~Camera()
{
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetInverseViewMatrix()
{
	return invViewMatrix;
}

XMFLOAT4X4 Camera::GetProjMatrix()
{
	return projMatrix;
}

XMFLOAT4X4 Camera::GetInverseProjMatrix()
{
	return invProjMatrix;
}

void Camera::SetProjMatrix(XMFLOAT4X4 pm)
{
	projMatrix = pm;
}

void Camera::SetViewMatrix(XMFLOAT4X4 vm)
{
	viewMatrix = vm;
}

void Camera::RotateCamera(float x, float y, float z)
{
	xRotation += (float)y;// / 100.0f;
	yRotation += (float)x;// / 100.0f;
	zRotation += (float)z;// / 100.0f;

	if (xRotation > (89.0f * XM_PI) / 180.0f) xRotation = (89.0f * XM_PI) / 180.0f;
	if (xRotation < (-89.0f * XM_PI) / 180.0f) xRotation = (-89.0f * XM_PI) / 180.0f;
	if (yRotation > XM_2PI) yRotation = yRotation - XM_2PI;
	if (yRotation < -XM_2PI) yRotation = yRotation + XM_2PI;
}

void Camera::SetPosition(XMFLOAT3 pos)
{
	position = pos;
}

void Camera::SetFOV(float degrees)
{
	fov = (3.1415926535f / 180.0f) * degrees;
	UpdateProjectionMatrix();
}

void Camera::UpdateProjectionMatrix()
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		fov,													// Field of View Angle
		(float)Config::ViewPortWidth / Config::ViewPortHeight,	// Aspect ratio
		nearClip,												// Near clip plane distance
		farClip);												// Far clip plane distance
	XMStoreFloat4x4(&projMatrix, XMMatrixTranspose(P));			// Transpose for HLSL!

	XMStoreFloat4x4(&invProjMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, P)));
}

void Camera::Update()
{
	XMFLOAT3 zAxis(0.0f, 0.0f, 1.0f);
	XMFLOAT3 yAxis(0.0f, 1.0f, 0.0f);

	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction);
	XMVECTOR right = XMVector3Cross(dir, XMLoadFloat3(&yAxis));

	float scalar = 30;

	if (Config::DebugCamera) {
		//* Can now use the new input system instead!
		if (GetAsyncKeyState('W') & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(dir, 0.05f * scalar));
			XMStoreFloat3(&position, pos);
		}
		if (GetAsyncKeyState('S') & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(dir, -0.05f * scalar));
			XMStoreFloat3(&position, pos);
		}
		if (GetAsyncKeyState('A') & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(right, 0.05f * scalar));
			XMStoreFloat3(&position, pos);
		}
		if (GetAsyncKeyState('D') & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(right, -0.05f * scalar));
			XMStoreFloat3(&position, pos);
		}

		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(XMLoadFloat3(&yAxis), 0.05f));
			XMStoreFloat3(&position, pos);
		}
		if (GetAsyncKeyState('X') & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(XMLoadFloat3(&yAxis), -0.05f));
			XMStoreFloat3(&position, pos);
		}
		if (GetAsyncKeyState('K') & 0x8000) {
			RotateCamera(0, 0, 1);
		}
		if (GetAsyncKeyState('L') & 0x8000) {
			RotateCamera(0, 0, -1);
		}

		if (mouse->OnLMBDown()) {
			prevMousePos.x = mouse->GetPosX();
			prevMousePos.y = mouse->GetPosY();
		}

		if (mouse->LMBIsPressed()) {
			RotateCamera((float)(mouse->GetPosX() - (int)prevMousePos.x) / 100.0f, (float)(mouse->GetPosY() - (int)prevMousePos.y) / 100.0f);
			prevMousePos.x = mouse->GetPosX();
			prevMousePos.y = mouse->GetPosY();
		}

	}
	

	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(xRotation, yRotation, zRotation);
	XMVECTOR newDir = XMVector3Rotate(XMLoadFloat3(&zAxis), quat);
	XMVECTOR newUp = XMVector3Rotate(XMLoadFloat3(&yAxis), quat);
	XMMATRIX view = XMMatrixLookToLH(pos, dir, newUp);
	XMMATRIX inverseView = XMMatrixInverse(nullptr, view);

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
	XMStoreFloat4x4(&invViewMatrix, XMMatrixTranspose(inverseView));
	XMStoreFloat3(&direction, newDir);

	//cout << "Pos: (" << position.x << ", " << position.y << ", " << position.z << ")" << endl;
	//cout << "Dir: (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << endl;
	//cout << "Rot: (" << xRotation << ", " << yRotation << ")" << endl << endl;
}
