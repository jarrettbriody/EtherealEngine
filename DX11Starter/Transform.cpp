#include "pch.h"
#include "Transform.h"

void Transform::CalcPosRotScale()
{
	XMVECTOR trns;
	XMVECTOR rot;
	XMVECTOR scl;

	XMMATRIX world = XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix));

	XMMatrixDecompose(&scl, &rot, &trns, world);

	XMStoreFloat3(&position, trns);
	XMStoreFloat4(&quaternion, rot);
	XMStoreFloat3(&scale, scl);

	CalcEulerAngles();
	CalcDirectionVector();
}

void Transform::CalcEulerAngles()
{
	XMVECTOR q = XMQuaternionNormalize(XMLoadFloat4(&quaternion));

	XMFLOAT3 x = X_AXIS;
	XMFLOAT3 y = Y_AXIS;
	XMFLOAT3 z = Z_AXIS;

	XMVECTOR xs = XMLoadFloat3(&x);
	XMVECTOR ys = XMLoadFloat3(&y);
	XMVECTOR zs = XMLoadFloat3(&z);

	XMQuaternionToAxisAngle(&xs, &rotation.x, q);
	XMQuaternionToAxisAngle(&ys, &rotation.y, q);
	XMQuaternionToAxisAngle(&zs, &rotation.z, q);

	rotationInDegrees = XMFLOAT3(DirectX::XMConvertToDegrees(rotation.x), DirectX::XMConvertToDegrees(rotation.y), DirectX::XMConvertToDegrees(rotation.z));
}

void Transform::CalcQuaternion()
{
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMStoreFloat4(&quaternion, XMQuaternionNormalize(quat));
}

void Transform::CalcDirectionVector()
{
	XMFLOAT3 zAxis = Z_AXIS;
	XMFLOAT3 yAxis = Y_AXIS;
	XMFLOAT3 xAxis = X_AXIS;
	XMVECTOR dir = XMLoadFloat3(&zAxis);
	XMVECTOR u = XMLoadFloat3(&yAxis);
	XMVECTOR r = XMLoadFloat3(&xAxis);
	XMVECTOR quat = XMLoadFloat4(&quaternion);
	dir = XMVector3Transform(dir, XMMatrixRotationQuaternion(quat));
	u = XMVector3Transform(u, XMMatrixRotationQuaternion(quat));
	r = XMVector3Transform(r, XMMatrixRotationQuaternion(quat));
	XMStoreFloat3(&direction, dir);
	XMStoreFloat3(&up, u);
	XMStoreFloat3(&right, r);
}

void Transform::CalcWorldMatrix()
{
	XMMATRIX trans = XMMatrixTranslation(position.x, position.y, position.z);
	XMVECTOR quat = XMLoadFloat4(&quaternion);
	XMMATRIX rot = XMMatrixRotationQuaternion(quat);
	XMMATRIX scl = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX world = scl * rot * trans;
	if (parent != nullptr) {
		XMMATRIX parentWorld = XMMatrixTranspose(XMLoadFloat4x4(&parent->worldMatrix));
		world = XMMatrixMultiply(world, parentWorld);
	}
	else if (parentWorld != nullptr) {
		XMFLOAT4X4 parentW = *parentWorld;
		XMMATRIX parentWorld = XMMatrixTranspose(XMLoadFloat4x4(&parentW));
		world = XMMatrixMultiply(world, parentWorld);
	}

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(world));

	XMStoreFloat4x4(&invWorldMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, world)));

	for (size_t i = 0; i < children.Count(); i++)
	{
		children[i]->CalcWorldMatrix();
	}

	if (updateCallback != nullptr) {
		updateCallback->Call();
	}
}

Transform::Transform(unsigned int flags)
{
	this->flags = flags;
}

Transform::~Transform()
{
	children.Cleanup();
}

void Transform::operator= (const Transform& other) {
	flags = other.flags;

	parent = other.parent;
	parentWorld = other.parentWorld;
	children = other.children;

	worldMatrix = other.worldMatrix;
	invWorldMatrix = other.invWorldMatrix;

	position = other.position;

	quaternion = other.quaternion;
	rotation = other.rotation;
	rotationInDegrees = other.rotationInDegrees;

	scale = other.scale;

	direction = other.direction;
	up = other.up;
	right = other.right;

	updateCallback = nullptr;
}

void Transform::Cleanup()
{
	children.Cleanup();
}

void Transform::SetParent(Transform* parent, bool preserveChild)
{
	this->parent = parent;
	if (preserveChild) {
		XMMATRIX invParentWorld = XMMatrixTranspose(XMLoadFloat4x4(&parent->invWorldMatrix));
		XMFLOAT4X4 newMat;
		XMStoreFloat4x4(&newMat, XMMatrixTranspose(XMMatrixMultiply(XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)), invParentWorld)));
		SetWorldMatrix(newMat);
	}
	CalcWorldMatrix();
}

void Transform::SetParent(XMFLOAT4X4* parent, bool preserveChild)
{
	this->parentWorld = parent;
	if (preserveChild) {
		XMMATRIX invParentWorld = XMMatrixInverse(nullptr, XMMatrixTranspose(XMLoadFloat4x4(parent)));
		XMFLOAT4X4 newMat;
		XMStoreFloat4x4(&newMat, XMMatrixTranspose(XMMatrixMultiply(XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)), invParentWorld)));
		SetWorldMatrix(newMat);
	}
	CalcWorldMatrix();
}

void Transform::AddChild(Transform* child, bool preserveChild)
{
	//children.InitBuffer();
	children.Push(child);
	child->SetParent(this, preserveChild);
}

void Transform::SetUpdateCallback(Callback* cb)
{
	updateCallback = cb;
}

void Transform::SetWorldMatrix(XMFLOAT4X4 matrix)
{
	worldMatrix = matrix;
	XMStoreFloat4x4(&invWorldMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)))));
	CalcPosRotScale();
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	return worldMatrix;
}

XMFLOAT4X4 Transform::GetInverseWorldMatrix()
{
	return invWorldMatrix;
}

XMFLOAT4X4* Transform::GetWorldMatrixPtr()
{
	return &worldMatrix;
}

XMFLOAT3 Transform::GetPosition()
{
	if((flags | (unsigned int)TRANSFORM_FLAGS::POSITION) == flags)
		return position;
	return ZERO_VECTOR3;
}

void Transform::SetPosition(XMFLOAT3 p)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::POSITION) == flags) {
		position = p;
		CalcWorldMatrix();
	}
}

void Transform::SetPosition(float x, float y, float z)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::POSITION) == flags) {
		position = XMFLOAT3(x, y, z);
		CalcWorldMatrix();
	}
}

void Transform::Move(XMFLOAT3 f)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::POSITION) == flags) {
		XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&f)));
		CalcWorldMatrix();
	}
}

void Transform::Move(float x, float y, float z)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::POSITION) == flags) {
		XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), XMVectorSet(x, y, z, 0)));
		CalcWorldMatrix();
	}
}

XMFLOAT4 Transform::GetRotationQuaternion()
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::ROTATION) == flags)
		return quaternion;
	return QUATERNION_IDENTITY;
}

XMFLOAT3 Transform::GetEulerAnglesRadians()
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::ROTATION) == flags)
		return rotation;
	return ZERO_VECTOR3;
}

XMFLOAT3 Transform::GetEulerAnglesDegrees()
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::ROTATION) == flags)
		return rotationInDegrees;
	return ZERO_VECTOR3;
}

void Transform::SetRotationQuaternion(XMFLOAT4 quat)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::ROTATION) == flags) {
		quaternion = quat;

		CalcEulerAngles();
		CalcDirectionVector();
		CalcWorldMatrix();
	}
}

void Transform::SetRotationRadians(XMFLOAT3 rotRadians)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::ROTATION) == flags) {
		rotation = rotRadians;

		rotationInDegrees.x = DirectX::XMConvertToDegrees(rotation.x);
		rotationInDegrees.y = DirectX::XMConvertToDegrees(rotation.y);
		rotationInDegrees.z = DirectX::XMConvertToDegrees(rotation.z);

		CalcQuaternion();
		CalcDirectionVector();
		CalcWorldMatrix();
	}
}

void Transform::SetRotationRadians(float x, float y, float z)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::ROTATION) == flags) {
		rotation = XMFLOAT3(x, y, z);

		rotationInDegrees.x = DirectX::XMConvertToDegrees(x);
		rotationInDegrees.y = DirectX::XMConvertToDegrees(y);
		rotationInDegrees.z = DirectX::XMConvertToDegrees(z);

		CalcQuaternion();
		CalcDirectionVector();
		CalcWorldMatrix();
	}
}

void Transform::SetRotationDegrees(XMFLOAT3 rotDegrees)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::ROTATION) == flags) {
		rotationInDegrees = rotDegrees;

		rotation.x = DirectX::XMConvertToRadians(rotDegrees.x);
		rotation.y = DirectX::XMConvertToRadians(rotDegrees.y);
		rotation.z = DirectX::XMConvertToRadians(rotDegrees.z);

		CalcQuaternion();
		CalcDirectionVector();
		CalcWorldMatrix();
	}
}

void Transform::SetRotationDegrees(float x, float y, float z)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::ROTATION) == flags) {
		rotationInDegrees = XMFLOAT3(x, y, z);

		rotation.x = DirectX::XMConvertToRadians(x);
		rotation.y = DirectX::XMConvertToRadians(y);
		rotation.z = DirectX::XMConvertToRadians(z);

		CalcQuaternion();
		CalcDirectionVector();
		CalcWorldMatrix();
	}
}

void Transform::RotateAroundAxis(XMFLOAT3 axis, float scalar)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::ROTATION) == flags) {
		XMVECTOR a = XMLoadFloat3(&axis);
		XMVECTOR quat = XMQuaternionNormalize(XMQuaternionRotationAxis(a, scalar));
		XMVECTOR existingQuat = XMQuaternionNormalize(XMLoadFloat4(&quaternion));
		XMVECTOR result = XMQuaternionMultiply(quat, existingQuat);
		XMStoreFloat4(&quaternion, XMQuaternionNormalize(result));

		CalcEulerAngles();
		CalcDirectionVector();
		CalcWorldMatrix();
	}
}

XMFLOAT3 Transform::GetScale()
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::SCALE) == flags)
		return scale;
	return ZERO_VECTOR3;
}

void Transform::SetScale(XMFLOAT3 s)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::SCALE) == flags) {
		scale = s;
		CalcWorldMatrix();
	}
}

void Transform::SetScale(float x, float y, float z)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::SCALE) == flags) {
		scale = XMFLOAT3(x, y, z);
		CalcWorldMatrix();
	}
}

XMFLOAT3 Transform::GetDirectionVector()
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::LOCALAXIS) == flags)
		return direction;
	return ZERO_VECTOR3;
}

XMFLOAT3 Transform::GetUpVector()
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::LOCALAXIS) == flags)
		return up;
	return ZERO_VECTOR3;
}

XMFLOAT3 Transform::GetRightVector()
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::LOCALAXIS) == flags)
		return right;
	return ZERO_VECTOR3;
}

void Transform::SetDirectionVector(XMFLOAT3 direction)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::LOCALAXIS) == flags) {
		XMVECTOR dir = XMLoadFloat3(&direction);
		dir = XMVector3Normalize(dir);
		XMStoreFloat3(&this->direction, dir);

		XMFLOAT3 y = Y_AXIS;
		XMVECTOR up = XMLoadFloat3(&y);
		XMVECTOR right = XMVector3Cross(up, dir);
		right = XMVector3Normalize(right);
		XMStoreFloat3(&this->right, right);
		up = XMVector3Cross(dir, right);
		up = XMVector3Normalize(up);
		XMStoreFloat3(&this->up, up);
		XMFLOAT4 botRow(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR trans = XMLoadFloat4(&botRow);
		XMMATRIX rotation = XMMATRIX(right, up, dir, trans);

		XMVECTOR quat = XMQuaternionRotationMatrix(rotation);

		XMStoreFloat4(&quaternion, XMQuaternionNormalize(quat));

		CalcEulerAngles();
		CalcWorldMatrix();
	}
}

void Transform::SetDirectionVectorU(XMFLOAT3 direction, XMFLOAT3 up)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::LOCALAXIS) == flags) {
		XMVECTOR dir = XMLoadFloat3(&direction);
		dir = XMVector3Normalize(dir);
		XMStoreFloat3(&this->direction, dir);

		XMVECTOR upC = XMLoadFloat3(&up);
		upC = XMVector3Normalize(upC);
		XMStoreFloat3(&this->up, upC);

		XMVECTOR right = XMVector3Cross(upC, dir);
		right = XMVector3Normalize(right);
		XMStoreFloat3(&this->right, right);

		XMFLOAT4 botRow(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR trans = XMLoadFloat4(&botRow);
		XMMATRIX rotation = XMMATRIX(right, upC, dir, trans);

		XMVECTOR quat = XMQuaternionRotationMatrix(rotation);

		XMStoreFloat4(&quaternion, XMQuaternionNormalize(quat));

		CalcEulerAngles();
		CalcWorldMatrix();
	}
}

void Transform::SetDirectionVectorR(XMFLOAT3 direction, XMFLOAT3 right)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::LOCALAXIS) == flags) {
		XMVECTOR dir = XMLoadFloat3(&direction);
		dir = XMVector3Normalize(dir);
		XMStoreFloat3(&this->direction, dir);

		XMVECTOR rightC = XMLoadFloat3(&right);
		rightC = XMVector3Normalize(rightC);
		XMStoreFloat3(&this->right, rightC);

		XMVECTOR up = XMVector3Cross(dir, rightC);
		up = XMVector3Normalize(up);
		XMStoreFloat3(&this->up, up);

		XMFLOAT4 botRow(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR trans = XMLoadFloat4(&botRow);
		XMMATRIX rotation = XMMATRIX(rightC, up, dir, trans);

		XMVECTOR quat = XMQuaternionRotationMatrix(rotation);

		XMStoreFloat4(&quaternion, XMQuaternionNormalize(quat));

		CalcEulerAngles();
		CalcWorldMatrix();
	}
}

void Transform::SetDirectionVectorUR(XMFLOAT3 direction, XMFLOAT3 up, XMFLOAT3 right)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::LOCALAXIS) == flags) {
		XMVECTOR dir = XMLoadFloat3(&direction);
		dir = XMVector3Normalize(dir);
		XMStoreFloat3(&this->direction, dir);

		XMVECTOR upC = XMLoadFloat3(&up);
		upC = XMVector3Normalize(upC);
		XMStoreFloat3(&this->up, upC);

		XMVECTOR rightC = XMLoadFloat3(&right);
		rightC = XMVector3Normalize(rightC);
		XMStoreFloat3(&this->right, rightC);

		XMFLOAT4 botRow(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR trans = XMLoadFloat4(&botRow);
		XMMATRIX rotation = XMMATRIX(rightC, upC, dir, trans);

		XMVECTOR quat = XMQuaternionRotationMatrix(rotation);

		XMStoreFloat4(&quaternion, XMQuaternionNormalize(quat));

		CalcEulerAngles();
		CalcWorldMatrix();
	}
}

void Transform::SetUpVector(XMFLOAT3 up)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::LOCALAXIS) == flags) {
		XMVECTOR upC = XMLoadFloat3(&up);
		upC = XMVector3Normalize(upC);
		XMStoreFloat3(&this->up, upC);

		XMFLOAT3 z = Z_AXIS;
		XMVECTOR dir = XMLoadFloat3(&z);
		XMVECTOR right = XMVector3Cross(upC, dir);
		right = XMVector3Normalize(right);
		XMStoreFloat3(&this->right, right);
		dir = XMVector3Cross(upC, right);
		dir = XMVector3Normalize(dir);
		XMStoreFloat3(&this->direction, dir);
		XMFLOAT4 botRow(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR trans = XMLoadFloat4(&botRow);
		XMMATRIX rotation = XMMATRIX(right, upC, dir, trans);

		XMVECTOR quat = XMQuaternionRotationMatrix(rotation);

		XMStoreFloat4(&quaternion, XMQuaternionNormalize(quat));

		CalcEulerAngles();
		CalcWorldMatrix();
	}
}

void Transform::SetRightVector(XMFLOAT3 right)
{
	if ((flags | (unsigned int)TRANSFORM_FLAGS::LOCALAXIS) == flags) {
		XMVECTOR rightC = XMLoadFloat3(&right);
		rightC = XMVector3Normalize(rightC);
		XMStoreFloat3(&this->right, rightC);

		XMFLOAT3 z = Z_AXIS;
		XMVECTOR dir = XMLoadFloat3(&z);
		XMVECTOR up = XMVector3Cross(dir, rightC);
		up = XMVector3Normalize(up);
		XMStoreFloat3(&this->up, up);
		dir = XMVector3Cross(up, rightC);
		dir = XMVector3Normalize(dir);
		XMStoreFloat3(&this->direction, dir);
		XMFLOAT4 botRow(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR trans = XMLoadFloat4(&botRow);
		XMMATRIX rotation = XMMATRIX(rightC, up, dir, trans);

		XMVECTOR quat = XMQuaternionRotationMatrix(rotation);

		XMStoreFloat4(&quaternion, XMQuaternionNormalize(quat));

		CalcEulerAngles();
		CalcWorldMatrix();
	}
}
