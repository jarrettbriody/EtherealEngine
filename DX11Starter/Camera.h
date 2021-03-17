#pragma once
#include "pch.h"
#include "Config.h"

using namespace DirectX;
using namespace std;

class Camera
{
private:
	XMFLOAT4X4 viewMatrix = XMFLOAT4X4();
	XMFLOAT4X4 invViewMatrix = XMFLOAT4X4();
	XMFLOAT4X4 projMatrix = XMFLOAT4X4();
	float fov = 0.5f * 3.1415926535f;
	float nearClip = 0.1f;
	float farClip = 1000.0f;
public:
	XMFLOAT3 position;
	XMFLOAT3 direction;
	float xRotation;
	float yRotation;
	Camera();
	~Camera();
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetInverseViewMatrix();
	XMFLOAT4X4 GetProjMatrix();
	void SetProjMatrix(XMFLOAT4X4 pm);
	void SetViewMatrix(XMFLOAT4X4 vm);
	void RotateCamera(int x, int y);
	void SetPosition(XMFLOAT3 pos);
	void UpdateProjectionMatrix();
	void Update();
};

