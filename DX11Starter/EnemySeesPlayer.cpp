#include "pch.h"
#include "EnemySeesPlayer.h"

void EnemySeesPlayer::OnInitialize()
{
	//cout << "Enemy: " << enemy->GetName() << " | View Angle: " << viewAngle << " | View Distance: " << viewDistance << endl;
}

void EnemySeesPlayer::OnTerminate(Status s)
{

}

Status EnemySeesPlayer::Update()
{
	XMFLOAT3 forward = CalculateEnemyForward();
	float currentAngle = atan2(forward.z, forward.x);
	float halfViewAngle = (XM_PI / 180.0f) * viewAngle / 2.0f;
	float hypotenuse = viewDistance / cos(halfViewAngle);

	XMVECTOR viewLeft = XMVectorSet(cos(currentAngle + halfViewAngle), 0.0f, sin(currentAngle + halfViewAngle), 0.0f) * hypotenuse;
	XMVECTOR viewRight = XMVectorSet(cos(currentAngle - halfViewAngle), 0.0f, sin(currentAngle - halfViewAngle), 0.0f) * hypotenuse;
	XMVECTOR viewFront = XMVectorSubtract(viewRight, viewLeft);
	XMFLOAT3 pos = player->GetPosition();
	XMVECTOR posVec = XMVectorSet(pos.x, 0.0f, pos.z, 1.0f);

	cout << "X: " << XMVectorGetX(viewLeft) << " | Y: " << XMVectorGetY(viewLeft) << " | Z: " << XMVectorGetZ(viewLeft) << " | W: " << XMVectorGetW(viewLeft) << endl;

	XMVECTOR result = PointInsideTriangle(posVec, viewLeft, viewFront, viewRight);

	//cout << "X: " << XMVectorGetX(result) << " | Y: " << XMVectorGetY(result) << " | Z: " << XMVectorGetZ(result) << " | W: " << XMVectorGetW(result) << endl;

	if (XMVectorGetX(result) == 1.0f && XMVectorGetY(result) == 1.0f && XMVectorGetZ(result) == 1.0f && XMVectorGetW(result) == 1.0f)
		return SUCCESS;
	else
		return FAILURE;
}

XMFLOAT3 EnemySeesPlayer::CalculateEnemyForward()
{
	XMFLOAT3 zAxis = Z_AXIS;
	XMVECTOR dir = XMLoadFloat3(&zAxis);
	dir = XMVector3Transform(dir, XMMatrixRotationQuaternion(XMLoadFloat4(&enemy->GetRotationQuaternion())));

	XMFLOAT3 direction;
	XMStoreFloat3(&direction, dir);

	return direction;
}

XMVECTOR EnemySeesPlayer::PointInsideTriangle(XMVECTOR p, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2)
{
	// Compute the triangle normal.
	XMVECTOR N = XMVector3Cross(XMVectorSubtract(v2, v0), XMVectorSubtract(v1, v0));

	// Compute the cross products of the vector from the base of each edge to
	// the point with each edge vector.
	XMVECTOR C0 = XMVector3Cross(XMVectorSubtract(p, v0), XMVectorSubtract(v1, v0));
	XMVECTOR C1 = XMVector3Cross(XMVectorSubtract(p, v1), XMVectorSubtract(v2, v1));
	XMVECTOR C2 = XMVector3Cross(XMVectorSubtract(p, v2), XMVectorSubtract(v0, v2));

	// If the cross product points in the same direction as the normal the the
	// point is inside the edge (it is zero if is on the edge).
	XMVECTOR Zero = XMVectorZero();
	XMVECTOR Inside0 = XMVectorGreaterOrEqual(XMVector3Dot(C0, N), Zero);
	XMVECTOR Inside1 = XMVectorGreaterOrEqual(XMVector3Dot(C1, N), Zero);
	XMVECTOR Inside2 = XMVectorGreaterOrEqual(XMVector3Dot(C2, N), Zero);

	// If the point inside all of the edges it is inside.
	return XMVectorAndInt(XMVectorAndInt(Inside0, Inside1), Inside2);
}