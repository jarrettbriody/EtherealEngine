#include "Camera.h"



Camera::Camera()
{
	position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	direction = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	xRotation = 0.0f;
	yRotation = 0.0f;
}


Camera::~Camera()
{
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjMatrix()
{
	return projMatrix;
}

void Camera::RotateCamera(int x, int y)
{
	xRotation += (float)y / 100.0f;
	yRotation += (float)x / 100.0f;

	if (xRotation > (89.0f * DirectX::XM_PI) / 180.0f) xRotation = (89.0f * DirectX::XM_PI) / 180.0f;
	if (xRotation < (-89.0f * DirectX::XM_PI) / 180.0f) xRotation = (-89.0f * DirectX::XM_PI) / 180.0f;
	if (yRotation > DirectX::XM_2PI) yRotation = yRotation - DirectX::XM_2PI;
	if (yRotation < -DirectX::XM_2PI) yRotation = yRotation + DirectX::XM_2PI;
}

void Camera::UpdateProjectionMatrix(int w, int h)
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)w / h,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	DirectX::XMStoreFloat4x4(&projMatrix, DirectX::XMMatrixTranspose(P)); // Transpose for HLSL!
}

void Camera::Update()
{
	DirectX::XMFLOAT3 zAxis(0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 yAxis(0.0f, 1.0f, 0.0f);

	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&direction);
	DirectX::XMVECTOR right = DirectX::XMVector3Cross(dir, DirectX::XMLoadFloat3(&yAxis));

	if (GetAsyncKeyState('W') & 0x8000) {
		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(dir,0.02f));
		DirectX::XMStoreFloat3(&position, pos);
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(dir, -0.02f));
		DirectX::XMStoreFloat3(&position, pos);
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(right, 0.02f));
		DirectX::XMStoreFloat3(&position, pos);
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(right, -0.02f));
		DirectX::XMStoreFloat3(&position, pos);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&yAxis), 0.02f));
		DirectX::XMStoreFloat3(&position, pos);
	}
	if (GetAsyncKeyState('X') & 0x8000) {
		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&yAxis), -0.02f));
		DirectX::XMStoreFloat3(&position, pos);
	}

	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(xRotation,yRotation,0.0f);
	DirectX::XMVECTOR newDir = DirectX::XMVector3Rotate(DirectX::XMLoadFloat3(&zAxis), quat);
	DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&position), dir, DirectX::XMLoadFloat3(&yAxis));

	DirectX::XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
	DirectX::XMStoreFloat3(&direction, newDir);
}
