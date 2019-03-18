#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
class Camera
{
private:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 direction;
	float xRotation;
	float yRotation;
public:
	Camera();
	~Camera();
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjMatrix();
	void RotateCamera(int x, int y);
	void UpdateProjectionMatrix(int w, int h);
	void Update();
};

