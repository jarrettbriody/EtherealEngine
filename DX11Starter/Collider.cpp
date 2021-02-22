#include "pch.h"
#include "Collider.h"

Collider::Collider()
{
}

Collider::Collider(vector<XMFLOAT3> vertices)
{
	//Count the points of the incoming list
	unsigned int vertCount = vertices.size();

	//If there are none just return, we have no information to create the BS from
	if (vertCount == 0)
		return;

	//Max and min as the first vector of the list
	maxLocal = minLocal = vertices[0];

	//Get the max and min out of the list
	for (unsigned int i = 1; i < vertCount; ++i)
	{
		if (maxLocal.x < vertices[i].x) maxLocal.x = vertices[i].x;
		else if (minLocal.x > vertices[i].x) minLocal.x = vertices[i].x;

		if (maxLocal.y < vertices[i].y) maxLocal.y = vertices[i].y;
		else if (minLocal.y > vertices[i].y) minLocal.y = vertices[i].y;

		if (maxLocal.z < vertices[i].z) maxLocal.z = vertices[i].z;
		else if (minLocal.z > vertices[i].z) minLocal.z = vertices[i].z;
	}

	//with model matrix being the identity, local and global are the same
	minGlobal = minLocal;
	maxGlobal = maxLocal;

	//Calculate the 8 corners of the cube
	//Back square
	untransformedColliderCorners[0] = colliderCorners[0] = minLocal;
	untransformedColliderCorners[1] = colliderCorners[1] = XMFLOAT3(maxLocal.x, minLocal.y, minLocal.z);
	untransformedColliderCorners[2] = colliderCorners[2] = XMFLOAT3(maxLocal.x, maxLocal.y, minLocal.z);
	untransformedColliderCorners[3] = colliderCorners[3] = XMFLOAT3(minLocal.x, maxLocal.y, minLocal.z);

	//Front square
	untransformedColliderCorners[4] = colliderCorners[4] = XMFLOAT3(minLocal.x, maxLocal.y, maxLocal.z);
	untransformedColliderCorners[5] = colliderCorners[5] = XMFLOAT3(minLocal.x, minLocal.y, maxLocal.z);
	untransformedColliderCorners[6] = colliderCorners[6] = XMFLOAT3(maxLocal.x, minLocal.y, maxLocal.z);
	untransformedColliderCorners[7] = colliderCorners[7] = maxLocal;

	XMVECTOR minLoc = XMLoadFloat3(&minLocal);
	XMVECTOR maxLoc = XMLoadFloat3(&maxLocal);

	//with the max and the min we calculate the center
	XMStoreFloat3(&centerLocal, XMVectorAdd(maxLoc, minLoc) / 2.0f);

	//we calculate the distance between min and max vectors
	XMStoreFloat3(&halfWidth, XMVectorSubtract(maxLoc, minLoc) / 2.0f);

	//Get the distance between the center and either the min or the max
	XMStoreFloat(&radius, XMVector3Length(XMLoadFloat3(&halfWidth)));

	for (size_t i = 0; i < 8; i++)
	{
		pivotShiftedColliderCorners[i] = XMFLOAT3(untransformedColliderCorners[i].x, untransformedColliderCorners[i].y, untransformedColliderCorners[i].z);// - 2 * centerLocal.z);
	}
}

Collider::~Collider()
{
}

void Collider::SetWorldMatrix(XMFLOAT4X4 worldMat)
{
	//Assign the model matrix
	worldMatrix = worldMat;

	XMMATRIX calculableWorldMatrix = XMMatrixTranspose(XMLoadFloat4x4(&worldMat));

	//back square
	colliderCorners[0] = minLocal;
	colliderCorners[1] = XMFLOAT3(maxLocal.x, minLocal.y, minLocal.z);
	colliderCorners[2] = XMFLOAT3(maxLocal.x, maxLocal.y, minLocal.z);
	colliderCorners[3] = XMFLOAT3(minLocal.x, maxLocal.y, minLocal.z);

	//Front square
	colliderCorners[4] = XMFLOAT3(minLocal.x, maxLocal.y, maxLocal.z);
	colliderCorners[5] = XMFLOAT3(minLocal.x, minLocal.y, maxLocal.z);
	colliderCorners[6] = XMFLOAT3(maxLocal.x, minLocal.y, maxLocal.z);
	colliderCorners[7] = maxLocal;

	XMVECTOR calculableCenterGlobal = XMLoadFloat3(&centerLocal);
	calculableCenterGlobal = XMVector4Transform(calculableCenterGlobal, calculableWorldMatrix);
	XMStoreFloat3(&centerGlobal, calculableCenterGlobal);

	XMVECTOR calculableHalfWidth = XMLoadFloat3(&halfWidth);
	calculableHalfWidth = XMVector4Transform(calculableHalfWidth, calculableWorldMatrix);
	XMStoreFloat3(&halfWidthGlobal, calculableHalfWidth);

	//Place them in world space
	for (int i = 0; i < 8; i++)
	{
		XMFLOAT4 pt(pivotShiftedColliderCorners[i].x, pivotShiftedColliderCorners[i].y, pivotShiftedColliderCorners[i].z, 1.0f);
		XMVECTOR calculableCorner = XMLoadFloat4(&pt);
		XMStoreFloat3(&colliderCorners[i], XMVector4Transform(calculableCorner, calculableWorldMatrix));
	}

	//Identify the max and min as the first corner
	maxGlobal = minGlobal = colliderCorners[0];

	//get the new max and min for the global box
	for (int i = 1; i < 8; ++i)
	{
		if (maxGlobal.x < colliderCorners[i].x) maxGlobal.x = colliderCorners[i].x;
		else if (minGlobal.x > colliderCorners[i].x) minGlobal.x = colliderCorners[i].x;

		if (maxGlobal.y < colliderCorners[i].y) maxGlobal.y = colliderCorners[i].y;
		else if (minGlobal.y > colliderCorners[i].y) minGlobal.y = colliderCorners[i].y;

		if (maxGlobal.z < colliderCorners[i].z) maxGlobal.z = colliderCorners[i].z;
		else if (minGlobal.z > colliderCorners[i].z) minGlobal.z = colliderCorners[i].z;
	}

	//we calculate the distance between min and max vectors
	XMStoreFloat3(&span, XMVectorSubtract(XMLoadFloat3(&maxGlobal), XMLoadFloat3(&minGlobal)));

	XMFLOAT3 x = X_AXIS;
	XMFLOAT3 y = Y_AXIS;
	XMFLOAT3 z = Z_AXIS;

	XMVECTOR transformedX = XMLoadFloat3(&x);
	XMVECTOR transformedY = XMLoadFloat3(&y);
	XMVECTOR transformedZ = XMLoadFloat3(&z);

	transformedX = XMVector4Transform(transformedX, calculableWorldMatrix);
	transformedY = XMVector4Transform(transformedY, calculableWorldMatrix);
	transformedZ = XMVector4Transform(transformedZ, calculableWorldMatrix);

	transformedX = XMVector3Normalize(transformedX);
	transformedY = XMVector3Normalize(transformedY);
	transformedZ = XMVector3Normalize(transformedZ);

	XMStoreFloat3(&collisionProjVecs[0], transformedX);
	XMStoreFloat3(&collisionProjVecs[1], transformedY);
	XMStoreFloat3(&collisionProjVecs[2], transformedZ);
}
 
unsigned int Collider::CheckSATCollision(Collider* other)
{
	if (!collisionsEnabled || !other->collisionsEnabled) return 16;

	XMFLOAT3 axes[15];

	int index = 0;
	for (size_t i = 0; i < 3; i++)
	{
		axes[index] = collisionProjVecs[i];
		index++;
		axes[index] = other->collisionProjVecs[i];
		index++;
		for (size_t j = 0; j < 3; j++)
		{
			XMVECTOR vecThis = XMLoadFloat3(&collisionProjVecs[i]);
			XMVECTOR vecOther = XMLoadFloat3(&(other->collisionProjVecs[j]));
			XMVECTOR cross = XMVector3Cross(vecThis, vecOther);
			cross = XMVector3Normalize(cross);
			XMStoreFloat3(&axes[index], cross);
			index++;
		}
	}

	XMVECTOR axis;
	float objAMin;
	float objAMax; 
	float objBMin;
	float objBMax;
	XMVECTOR corner;
	XMVECTOR dot;
	float proj;

	for (size_t i = 0; i < 15; i++)
	{
		if (axes[i].x == 0.0f && axes[i].y == 0.0f && axes[i].z == 0.0f) {
			continue;
		}

		axis = XMLoadFloat3(&axes[i]);

		corner = XMLoadFloat3(&colliderCorners[0]);
		dot = XMVector3Dot(corner, axis);
		XMStoreFloat(&objAMin, dot);

		corner = XMLoadFloat3(&other->colliderCorners[0]);
		dot = XMVector3Dot(corner, axis);
		XMStoreFloat(&objBMin, dot);
		
		objAMax = objAMin;
		objBMax = objBMin;

		for (size_t j = 1; j < 8; j++)
		{
			corner = XMLoadFloat3(&colliderCorners[j]);
			dot = XMVector3Dot(corner, axis);
			XMStoreFloat(&proj, dot);
			if (proj > objAMax) objAMax = proj;
			else if (proj < objAMin) objAMin = proj;

			corner = XMLoadFloat3(&other->colliderCorners[j]);
			dot = XMVector3Dot(corner, axis);
			XMStoreFloat(&proj, dot);
			if (proj > objBMax) objBMax = proj;
			else if (proj < objBMin) objBMin = proj;
		}

		if (objAMin > objBMax || objAMax < objBMin) {
			return i;
		}
	}

	//there is no axis test that separates these two objects
	return -1;
}

bool Collider::CheckSATCollisionForCorrection(Collider* other, XMFLOAT3& result)
{
	if (!collisionsEnabled || !other->collisionsEnabled) return false;

	XMFLOAT3 axes[15];

	int index = 0;
	for (size_t i = 0; i < 3; i++)
	{
		axes[index] = collisionProjVecs[i];
		index++;
		axes[index] = other->collisionProjVecs[i];
		index++;
		for (size_t j = 0; j < 3; j++)
		{
			XMVECTOR vecThis = XMLoadFloat3(&collisionProjVecs[i]);
			XMVECTOR vecOther = XMLoadFloat3(&(other->collisionProjVecs[j]));
			XMVECTOR cross = XMVector3Cross(vecThis, vecOther);
			cross = XMVector3Normalize(cross);
			XMStoreFloat3(&axes[index], cross);
			index++;
		}
	}

	XMVECTOR axis;
	float objAMin;
	float objAMax;
	float objBMin;
	float objBMax;
	XMVECTOR corner;
	XMVECTOR dot;
	float proj;

	float scalar = 0.0f;
	XMVECTOR scalarAxis;

	for (size_t i = 0; i < 15; i++)
	{
		if (axes[i].x == 0.0f && axes[i].y == 0.0f && axes[i].z == 0.0f) {
			continue;
		}

		axis = XMLoadFloat3(&axes[i]);

		corner = XMLoadFloat3(&colliderCorners[0]);
		dot = XMVector3Dot(corner, axis);
		XMStoreFloat(&objAMin, dot);

		corner = XMLoadFloat3(&other->colliderCorners[0]);
		dot = XMVector3Dot(corner, axis);
		XMStoreFloat(&objBMin, dot);

		objAMax = objAMin;
		objBMax = objBMin;

		for (size_t j = 1; j < 8; j++)
		{
			corner = XMLoadFloat3(&colliderCorners[j]);
			dot = XMVector3Dot(corner, axis);
			XMStoreFloat(&proj, dot);
			if (proj > objAMax) objAMax = proj;
			else if (proj < objAMin) objAMin = proj;

			corner = XMLoadFloat3(&other->colliderCorners[j]);
			dot = XMVector3Dot(corner, axis);
			XMStoreFloat(&proj, dot);
			if (proj > objBMax) objBMax = proj;
			else if (proj < objBMin) objBMin = proj;
		}

		if (objAMin > objBMax || objAMax < objBMin) {
			return false;
		}
		else if(objAMin < objBMax || objAMax > objBMin){
			if (scalar == 0.0f) {
				scalar = objBMax - objAMin;
				scalarAxis = axis;
			}
			else if (abs(objBMax - objAMin) < abs(scalar)) {
				scalar = objBMax - objAMin;
				scalarAxis = axis;
			}
			else if (abs(objAMax - objBMin) < abs(scalar)) {
				scalar = objAMax - objBMin;
				scalarAxis = axis;
			}
		}
	}

	//there is no axis test that separates these two objects
	XMFLOAT3 dist;
	XMVECTOR vs = XMVectorScale(scalarAxis, (scalar + 0.0001f));
	XMStoreFloat3(&dist, vs);
	result = dist;
	return true;
}

XMFLOAT3* Collider::GetColliderCorners()
{
	return colliderCorners;
}

XMFLOAT3* Collider::GetUntransformedColliderCorners()
{
	return untransformedColliderCorners;
}

XMFLOAT3* Collider::GetPivotShiftedColliderCorners()
{
	return pivotShiftedColliderCorners;
}

XMFLOAT4X4 Collider::GetWorldMatrix()
{
	return worldMatrix;
}

XMFLOAT3 Collider::GetSpan() 
{
	return span;
}

XMFLOAT3 Collider::GetHalfWidth()
{
	return halfWidth;
}

XMFLOAT3 Collider::GetHalfWidthGlobal()
{
	return halfWidthGlobal;
}

XMFLOAT3 Collider::GetCenterLocal()
{
	return centerLocal;
}

XMFLOAT3 Collider::GetCenterGlobal()
{
	return centerGlobal;
}
