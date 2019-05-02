#include "Camera.h"



Camera::Camera()
{
	position = XMFLOAT3(0.0f, 5.0f, -10.0f);
	direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	XMVECTOR dir = XMLoadFloat3(&direction);
	dir = XMVector3Normalize(dir);
	XMStoreFloat3(&direction, dir);
	xRotation = 0.0f;
	yRotation = 0.0f;
}


Camera::~Camera()
{
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjMatrix()
{
	return projMatrix;
}

void Camera::SetProjMatrix(XMFLOAT4X4 pm)
{
	projMatrix = pm;
}

void Camera::SetViewMatrix(XMFLOAT4X4 vm)
{
	viewMatrix = vm;
}

void Camera::RotateCamera(int x, int y)
{
	xRotation += (float)y / 100.0f;
	yRotation += (float)x / 100.0f;

	if (xRotation > (89.0f * XM_PI) / 180.0f) xRotation = (89.0f * XM_PI) / 180.0f;
	if (xRotation < (-89.0f * XM_PI) / 180.0f) xRotation = (-89.0f * XM_PI) / 180.0f;
	if (yRotation > XM_2PI) yRotation = yRotation - XM_2PI;
	if (yRotation < -XM_2PI) yRotation = yRotation + XM_2PI;
}

void Camera::UpdateProjectionMatrix(int w, int h)
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.4f * 3.1415926535f,		// Field of View Angle
		(float)w / h,				// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

void Camera::Update()
{
	XMFLOAT3 zAxis(0.0f, 0.0f, 1.0f);
	XMFLOAT3 yAxis(0.0f, 1.0f, 0.0f);

	XMVECTOR pos = XMLoadFloat3(&position);
	XMVECTOR dir = XMLoadFloat3(&direction);
	XMVECTOR right = XMVector3Cross(dir, XMLoadFloat3(&yAxis));

	if (GetAsyncKeyState('W') & 0x8000) {
		pos = XMVectorAdd(pos, XMVectorScale(dir,0.05f));
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
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		pos = XMVectorAdd(pos, XMVectorScale(XMLoadFloat3(&yAxis), 0.05f));
		XMStoreFloat3(&position, pos);
	}
	if (GetAsyncKeyState('X') & 0x8000) {
		pos = XMVectorAdd(pos, XMVectorScale(XMLoadFloat3(&yAxis), -0.05f));
		XMStoreFloat3(&position, pos);
	}

	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(xRotation,yRotation,0.0f);
	XMVECTOR newDir = XMVector3Rotate(XMLoadFloat3(&zAxis), quat);
	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&position), dir, XMLoadFloat3(&yAxis));

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
	XMStoreFloat3(&direction, newDir);

	/*
	cout << "Pos: (" << position.x << ", " << position.y << ", " << position.z << ")" << endl;
	cout << "Dir: (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << endl;
	cout << "Rot: (" << xRotation << ", " << yRotation << ")" << endl << endl;
	*/
}
