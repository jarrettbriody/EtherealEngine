#pragma once
#include "pch.h"
#include "Config.h"
#include "Mouse.h"
#include "Transform.h"

using namespace DirectX;
using namespace std;

class Camera
{
private:
	Transform transform;
	XMFLOAT4X4 viewMatrix = MATRIX_IDENTITY;
	XMFLOAT4X4 invViewMatrix = MATRIX_IDENTITY;
	XMFLOAT4X4 projMatrix = MATRIX_IDENTITY;
	XMFLOAT4X4 invProjMatrix = MATRIX_IDENTITY;
	float fov = 0.5f * 3.1415926535f;
	float nearClip = 0.1f;
	float farClip = 2000.0f;
	Mouse* mouse = nullptr;
	POINT prevMousePos;

public:
	XMFLOAT3 rotation = ZERO_VECTOR3;
	XMFLOAT4 quaternion = QUATERNION_IDENTITY;
	Camera();
	~Camera();
	Transform& GetTransform();
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetInverseViewMatrix();
	XMFLOAT4X4 GetProjMatrix();
	XMFLOAT4X4 GetInverseProjMatrix();
	void SetProjMatrix(XMFLOAT4X4 pm);
	void SetViewMatrix(XMFLOAT4X4 vm);
	void RotateCamera(float x, float y, float z = 0.0f);
	void SetFOV(float degrees);
	void CalcViewMatrix();
	void CalcProjMatrix();
	void Update(double deltaTime);
};

