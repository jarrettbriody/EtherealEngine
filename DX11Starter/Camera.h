#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>

using namespace DirectX;
using namespace std;

class Camera
{
private:
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projMatrix;
	XMFLOAT3 position;
	XMFLOAT3 direction;
	float xRotation;
	float yRotation;
public:
	Camera();
	~Camera();
	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjMatrix();
	void SetProjMatrix(XMFLOAT4X4 pm);
	void SetViewMatrix(XMFLOAT4X4 vm);
	void RotateCamera(int x, int y);
	void UpdateProjectionMatrix(int w, int h);
	void Update();
};

