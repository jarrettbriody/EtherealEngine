#include "pch.h"
#include "Camera.h"


Camera::Camera()
{
	transform = Transform((unsigned int)TRANSFORM_FLAGS::POSITION | (unsigned int)TRANSFORM_FLAGS::ROTATION | (unsigned int)TRANSFORM_FLAGS::LOCALAXIS);

	transform.SetPosition(XMFLOAT3(-8.13f, 3.83f, -1.05f));

	rotation = XMFLOAT3(0, XMConvertToRadians(90.0f), 0);
	transform.SetRotationRadians(rotation);

	mouse = Mouse::GetInstance();
	prevMousePos = { 0, 0 };
}


Camera::~Camera()
{
}

Transform& Camera::GetTransform()
{
	return transform;
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
	rotation.x = rotation.x + y;// / 100.0f;
	rotation.y = rotation.y + x;// / 100.0f;
	rotation.z = rotation.z + z;// / 100.0f;

	if (rotation.x > (89.0f * XM_PI) / 180.0f) rotation.x = (89.0f * XM_PI) / 180.0f;
	if (rotation.x < (-89.0f * XM_PI) / 180.0f) rotation.x = (-89.0f * XM_PI) / 180.0f;
	if (rotation.y > XM_2PI) rotation.y = rotation.y - XM_2PI;
	if (rotation.y < -XM_2PI) rotation.y = rotation.y + XM_2PI;

	transform.SetRotationRadians(rotation);
	CalcViewMatrix();
}

void Camera::SetFOV(float degrees)
{
	fov = (3.1415926535f / 180.0f) * degrees;
	CalcProjMatrix();
}

void Camera::CalcProjMatrix()
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

void Camera::CalcViewMatrix()
{
	/*
	XMVECTOR pos = XMLoadFloat3(&transform.GetPosition());
	XMVECTOR d = XMLoadFloat3(&transform.GetDirectionVector());
	XMVECTOR u = XMLoadFloat3(&transform.GetUpVector());

	XMMATRIX view = XMMatrixLookToLH(pos, d, u);
	XMMATRIX inverseView = XMMatrixInverse(nullptr, view);

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
	XMStoreFloat4x4(&invViewMatrix, XMMatrixTranspose(inverseView));
	*/

	transform.SetRotationRadians(rotation);

	XMFLOAT3 zAxis(0.0f, 0.0f, 1.0f);
	XMFLOAT3 yAxis(0.0f, 1.0f, 0.0f);
	XMFLOAT3 newDire;
	XMFLOAT3 newR;
	XMFLOAT3 newU;

	XMVECTOR pos = XMLoadFloat3(&transform.GetPosition());
	XMVECTOR d = XMLoadFloat3(&transform.GetDirectionVector());
	XMVECTOR rightVec = XMVector3Cross(XMLoadFloat3(&yAxis), d);
	XMFLOAT4 quater = transform.GetRotationQuaternion();
	XMVECTOR quat = XMLoadFloat4(&quater);
	XMVECTOR newDir = XMVector3Rotate(XMLoadFloat3(&zAxis), quat);
	XMVECTOR newUp = XMVector3Rotate(XMLoadFloat3(&yAxis), quat);

	XMMATRIX view = XMMatrixLookToLH(pos, d, newUp);
	XMMATRIX inverseView = XMMatrixInverse(nullptr, view);

	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(view));
	XMStoreFloat4x4(&invViewMatrix, XMMatrixTranspose(inverseView));
	XMStoreFloat3(&newDire, newDir);
	XMStoreFloat3(&newR, rightVec);
	XMStoreFloat3(&newU, rightVec);
	//transform.SetDirectionVectorUR(newDire, newU, newR);
}

void Camera::Update(double deltaTime)
{
	XMVECTOR pos = XMLoadFloat3(&transform.GetPosition());
	XMVECTOR dir = XMLoadFloat3(&transform.GetDirectionVector());
	XMVECTOR right = XMLoadFloat3(&transform.GetRightVector());
	XMVECTOR up = XMLoadFloat3(&transform.GetUpVector());

	float scalar = Config::DebugCameraSpeed;

	XMFLOAT3 newPos;

	if (Config::DebugCamera) {
		//* Can now use the new input system instead!
		if (GetAsyncKeyState('W') & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(dir, scalar * deltaTime));
			XMStoreFloat3(&newPos, pos);
		}
		if (GetAsyncKeyState('S') & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(dir, -scalar * deltaTime));
			XMStoreFloat3(&newPos, pos);
		}
		if (GetAsyncKeyState('A') & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(right, -scalar * deltaTime));
			XMStoreFloat3(&newPos, pos);
		}
		if (GetAsyncKeyState('D') & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(right, scalar * deltaTime));
			XMStoreFloat3(&newPos, pos);
		}

		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(up, 0.5f * deltaTime));
			XMStoreFloat3(&newPos, pos);
		}
		if (GetAsyncKeyState('X') & 0x8000) {
			pos = XMVectorAdd(pos, XMVectorScale(up, -0.5f * deltaTime));
			XMStoreFloat3(&newPos, pos);
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

		transform.SetPosition(newPos);
	}

	CalcViewMatrix();

	//cout << "Pos: (" << position.x << ", " << position.y << ", " << position.z << ")" << endl;
	//cout << "Dir: (" << direction.x << ", " << direction.y << ", " << direction.z << ")" << endl;
	//cout << "Rot: (" << xRotation << ", " << yRotation << ")" << endl << endl;
}
