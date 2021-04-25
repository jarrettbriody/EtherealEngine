#pragma once
#include "pch.h"
#include "Config.h"
#include "Mouse.h"

using namespace DirectX;
using namespace std;

class Camera
{
private:
	XMFLOAT4X4 viewMatrix = XMFLOAT4X4();
	XMFLOAT4X4 invViewMatrix = XMFLOAT4X4();
	XMFLOAT4X4 projMatrix = XMFLOAT4X4();
	XMFLOAT4X4 invProjMatrix = XMFLOAT4X4();
	XMFLOAT4X4 worldMatrix = XMFLOAT4X4();
	XMFLOAT4X4 invWorldMatrix = XMFLOAT4X4();
	float fov = 0.5f * 3.1415926535f;
	float nearClip = 0.1f;
	float farClip = 2000.0f;
	Mouse* mouse;
	POINT prevMousePos;
public:
	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 right;
	float xRotation;
	float yRotation;
	float zRotation;
	Camera();
	~Camera();
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetInverseViewMatrix();
	XMFLOAT4X4 GetProjMatrix();
	XMFLOAT4X4 GetInverseProjMatrix();
	XMFLOAT4X4 GetWorldMatrix();
	XMFLOAT4X4 GetInvWorldMatrix();
	XMFLOAT4X4* GetWorldMatrixPtr();
	void SetProjMatrix(XMFLOAT4X4 pm);
	void SetViewMatrix(XMFLOAT4X4 vm);
	void RotateCamera(float x, float y, float z = 0.0f);
	void SetPosition(XMFLOAT3 pos);
	void SetFOV(float degrees);
	void UpdateProjectionMatrix();
	void CalcWorldMatrix();
	void Update();
};

