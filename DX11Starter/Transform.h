#pragma once
#include "pch.h"
#include "Config.h"

using namespace DirectX;
using namespace std;

enum class TRANSFORM_FLAGS {
	ALL = 0 - 1,
	POSITION = 1 << 31,
	ROTATION = 1 << 30,
	SCALE = 1 << 29,
	LOCALAXIS = 1 << 28,
	//STATIC = 1 << 1,
};

class Transform
{
private:
	unsigned int flags = 0;

	Transform* parent = nullptr;
	XMFLOAT4X4* parentWorld = nullptr;
	//vector<Transform*>* children = nullptr;

	XMFLOAT4X4 worldMatrix = MATRIX_IDENTITY;
	XMFLOAT4X4 invWorldMatrix = MATRIX_IDENTITY;

	XMFLOAT4X4 viewMatrix = MATRIX_IDENTITY;
	XMFLOAT4X4 invViewMatrix = MATRIX_IDENTITY;

	XMFLOAT4X4 projectionMatrix = MATRIX_IDENTITY;
	XMFLOAT4X4 invProjectionMatrix = MATRIX_IDENTITY;

	XMFLOAT3 position = ZERO_VECTOR3;

	XMFLOAT4 quaternion = QUATERNION_IDENTITY;
	XMFLOAT3 rotation = ZERO_VECTOR3;
	XMFLOAT3 rotationInDegrees = ZERO_VECTOR3;

	XMFLOAT3 scale = ONE_VECTOR3;

	XMFLOAT3 direction = Z_AXIS;
	XMFLOAT3 up = Y_AXIS;
	XMFLOAT3 right = X_AXIS;

	void CalcPosRotScale();
	void CalcEulerAngles();
	void CalcQuaternion();
	void CalcDirectionVector();
	void CalcWorldMatrix();

public:
	Transform(TRANSFORM_FLAGS flags = TRANSFORM_FLAGS::ALL);
	~Transform();

	void SetParent(Transform* parent, bool preserveChild = true);
	void SetParent(XMFLOAT4X4* parent, bool preserveChild = true);
	//void AddChild(Transform* child, bool preserveChild = true);

	void SetWorldMatrix(XMFLOAT4X4 matrix);
	XMFLOAT4X4 GetWorldMatrix();
	XMFLOAT4X4 GetInverseWorldMatrix();
	XMFLOAT4X4* GetWorldMatrixPtr();

	XMFLOAT3 GetPosition();
	void SetPosition(XMFLOAT3 p);
	void SetPosition(float x, float y, float z);
	void Move(XMFLOAT3 f);
	void Move(float x, float y, float z);

	XMFLOAT4 GetRotationQuaternion();
	XMFLOAT3 GetEulerAnglesRadians();
	XMFLOAT3 GetEulerAnglesDegrees(); 
	void SetRotationQuaternion(XMFLOAT4 quat);
	void SetRotationRadians(XMFLOAT3 rotRadians);
	void SetRotationRadians(float x, float y, float z);
	void SetRotationDegrees(XMFLOAT3 rotDegrees);
	void SetRotationDegrees(float x, float y, float z);
	void RotateAroundAxis(XMFLOAT3 axis, float scalar);

	XMFLOAT3 GetScale();
	void SetScale(XMFLOAT3 s);
	void SetScale(float x, float y, float z);

	XMFLOAT3 GetDirectionVector();
	XMFLOAT3 GetUpVector();
	XMFLOAT3 GetRightVector();
	void SetDirectionVector(XMFLOAT3 direction);
	void SetDirectionVectorU(XMFLOAT3 direction, XMFLOAT3 up);
	void SetDirectionVectorR(XMFLOAT3 direction, XMFLOAT3 right);
	void SetDirectionVectorUR(XMFLOAT3 direction, XMFLOAT3 up, XMFLOAT3 right);
	void SetUpVector(XMFLOAT3 up);
	void SetRightVector(XMFLOAT3 right);
};

